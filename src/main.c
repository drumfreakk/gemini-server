#include <stdio.h>
#include <tls.h>
#include <unistd.h>		/* close() */
#include <sys/socket.h>	/* SOCKETS (socket(), bind()...) */
#include <netinet/ip.h>	/* IPv4 () */
#include <arpa/inet.h>	/* network byte order */

#include "tools.h"
#include "logger.h"

int handle_connection(struct tls *client_tls, struct sockaddr_in *client_addr);

int main(){
	struct tls_config *config_tls;
	struct tls *client_tls, *server_tls;

	int host_fd, client_fd, yes = 1;
	struct sockaddr_in local_addr, client_addr;
	char tempstring[2048];


/* SETTING UP TLS */
	config_tls = tls_config_new();
	//Does it really work without tlsv1.2?
	if(tls_config_set_protocols(config_tls, TLS_PROTOCOL_TLSv1_3) == -1){
		fatal("Setting TLS protocols");
	}
	if(tls_config_set_keypair_file(config_tls, "/home/kip/gemini-server/keys/cert.pem",\
	                                         "/home/kip/gemini-server/keys/key.pem") == -1){
		fatal("Setting key/certificate");
	}

	server_tls = tls_server();
	if(server_tls == NULL) { fatal("Creating server"); }
	if(tls_configure(server_tls, config_tls) == -1) { fatal("Configuring server"); }

/* SETTING UP THE SOCKET */
	host_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(setsockopt(host_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		fatal("Setting socket options");

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1965);
	local_addr.sin_addr.s_addr = 0;	// Automatically sets to local address apparently

	if(bind(host_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) == -1){
		fatal("Binding socket");
	}

	if(listen(host_fd, 5) == -1) { fatal("Starting listening"); }

/* MAIN LOOP */
	while(1){
		socklen_t addrlen = sizeof(struct sockaddr_in);
		client_fd = accept(host_fd, (struct sockaddr *)&client_addr, &addrlen);
		if(client_fd == -1) { logger("SOCK", "Error accepting connection"); }

		sprintf(tempstring, "New connection from %s:%d", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		logger("DBG", tempstring);

		if(tls_accept_socket(server_tls, &client_tls, client_fd) == -1){
			logger("TLS", "Error upgrading to tls");
		}
		
		handle_connection(client_tls, &client_addr);
		
		tls_close(client_tls);
		tls_free(client_tls);

		close(client_fd);
	}
	tls_close(server_tls);
	tls_free(server_tls);
//TODO: properly clean up tls
//Should be fixed now?
	close(host_fd);

	return 0;
}

int handle_connection(struct tls *client_tls, struct sockaddr_in *client_addr){
	int recv_len, found_cr = 0;
	char *current_char;
	char buffer[1024], tempstring[2048];	//TODO remove the need for tempstring

	current_char = buffer;
		
	while(1){
		recv_len = tls_read(client_tls, current_char, 1);
		if(recv_len == 0){ 
			return -1;
		}
		else if(*current_char == '\0'){
			return -1;
		}
		else if(*current_char == '\n' && found_cr == 1){
			*(--current_char) = '\0';
			break;
		}
		else if(*current_char == '\r'){
			found_cr = 1;
		}

		if(current_char-buffer == 1023){	// Could be off by one
			return -1;
		}else{
			current_char++;
		}
	}
	sprintf(tempstring, "Request got from %s:%d: %s", \
		inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), buffer);
	logger("CONN", tempstring);
	
	dump(buffer, strlen(buffer));

	/* Check the protocol is actually gemini */
//		current_char = strpbrk(buffer, ":");
	if(strncmp(buffer, "gemini", 6)){
		logger("REQ", "Protocol is not gemini");
		return -1;
	}
	
	/* Check whether the hostname matches */
	//TODO implement this
	
	/* Split the path & query */
/*		Must it include a trailing slash if it doesnt have a path?
		reserved chars: ":" / "/" / "?" / "#" / "[" / "]" / "@"	
		URI       = scheme ":" hier-part [ "?" query ] [ "#" fragment ]
		hier-part = ( "//" authority ) \
		 			 ( path-abempty / path-absolute / path-rootless / path-empty )
		The authority component is preceded by a double slash ("//") and is \
		 terminated by the next slash ("/"), question mark ("?"), or number \
		 sign ("#") character, or by the end of the URI.
		If a URI contains an authority component, then the path component \
		 must either be empty or begin with a slash ("/") character.
*/
	char to_write[] = "40 aaa\r\n";
	if(tls_write(client_tls, to_write, 8) == -1) { logger("CONN", "Error sending message"); }

	return 0;
}
