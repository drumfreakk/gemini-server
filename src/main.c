#include <stdio.h>
#include <tls.h>
#include <unistd.h>		/* close() */
#include <sys/socket.h>	/* SOCKETS (socket(), bind()...) */
#include <netinet/ip.h>	/* IPv4 () */
#include <arpa/inet.h>	/* network byte order */

#include "tools.h"

int main(){
	struct tls_config *config_tls;
	struct tls *client_tls, *server_tls;

	int host_fd, client_fd, recv_len, yes = 1;
	struct sockaddr_in local_addr, client_addr;
	char buffer[1024], tempstring[2048];

	int found_cr, broken_request;
	char *current_char;

/* SETTING UP TLS */
	config_tls = tls_config_new();
	//Does it really work without tlsv1.2?
	if(tls_config_set_protocols(config_tls, TLS_PROTOCOL_TLSv1_3) == -1){
		fatal("setting TLS protocols");
	}
	if(tls_config_set_keypair_file(config_tls, "/home/kip/gemini-server/keys/cert.pem",\
	                                         "/home/kip/gemini-server/keys/key.pem") == -1){
		fatal("setting key/certificate");
	}

	server_tls = tls_server();
	if(server_tls == NULL) { fatal("creating server"); }
	if(tls_configure(server_tls, config_tls) == -1) { fatal("configuring server"); }

/* SETTING UP THE SOCKET */
	host_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(setsockopt(host_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		fatal("setting socket options");

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1965);
	local_addr.sin_addr.s_addr = 0;	// Automatically sets to local address apparently

	if(bind(host_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) == -1){
		fatal("binding socket");
	}

	if(listen(host_fd, 5) == -1) { fatal("starting listening"); }

/* MAIN LOOP */
	while(1){
		socklen_t addrlen = sizeof(struct sockaddr_in);
		client_fd = accept(host_fd, (struct sockaddr *)&client_addr, &addrlen);
		if(client_fd == -1) { error("accepting connection"); }

		sprintf(tempstring, "New connection from %s:%d", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		debug(tempstring);

		if(tls_accept_socket(server_tls, &client_tls, client_fd) == -1) { error("upgrading to tls"); }
		
		found_cr = 0, broken_request = 0;
		current_char = buffer;
		
		while(!broken_request){
			recv_len = tls_read(client_tls, current_char, 1);
			if(recv_len == 0){ 
				broken_request = 1; 
			}
			if(*current_char == '\n' && found_cr == 1){
				*(--current_char) = '\0';
				break;
			}
			if(*current_char == '\r'){
				found_cr = 1;
			}
			if(current_char-buffer == 1023){	// Could be off by one
				broken_request = 1;
			}else{
				current_char++;
			}
		}
		sprintf(tempstring, "Request got from %s:%d: %s", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
		info(tempstring);
		
		char to_write[] = "40 aaa\r\n";
		if(tls_write(client_tls, to_write, 8) == -1) { error("sending message"); }
		
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


