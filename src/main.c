#include <stdio.h>
#include <tls.h>
#include <unistd.h>		/* close() */
#include <sys/socket.h>	/* SOCKETS (socket(), bind()...) */
#include <netinet/ip.h>	/* IPv4 () */
#include <arpa/inet.h>	/* network byte order */

/* open() */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <string.h>		/* For dump() */

#include <errno.h>		/* errno */

#include "logger.h"

#define WEBROOT "/home/kip/gemini-server/webroot/"

//TODO Allow configuration via file


int handle_connection(struct tls *client_tls, struct sockaddr_in *client_addr);
int send_header(const int status, const char* meta, struct tls *client_tls); 

void dump(const char* string, const size_t len);

int main(){
	logger_config.level = LVL_INFO;
	logger_config.file[0] = '\0';

	struct tls_config *config_tls;
	struct tls *client_tls, *server_tls;

	int host_fd, client_fd, yes = 1;
	struct sockaddr_in local_addr, client_addr;
	char tempstring[2048];


/*	Set up TLS	*/
	config_tls = tls_config_new();
	if(tls_config_set_protocols(config_tls, TLS_PROTOCOL_TLSv1_3) == -1){
		logger(TG_TLS, "Setting TLS protocols", LG_FTL);
	}
	if(tls_config_set_keypair_file(config_tls, "/home/kip/gemini-server/keys/cert.pem",\
	                                         "/home/kip/gemini-server/keys/key.pem") == -1){
		logger(TG_TLS, "Setting key/certificate", LG_FTL);
	}

	server_tls = tls_server();
	if(server_tls == NULL) { logger(TG_TLS, "Creating server", LG_FTL); }
	if(tls_configure(server_tls, config_tls) == -1) { logger(TG_TLS, "Configuring server", LG_FTL); }

/*	Set up socket	*/
	host_fd = socket(AF_INET, SOCK_STREAM, 0);
	
	// Let ports that weren't marked as closed but are in disuse be used
	if(setsockopt(host_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		logger(TG_SOCK, "Setting socket options", LG_ERR | LG_FTL);

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1965);
	local_addr.sin_addr.s_addr = 0;	// Automatically sets to local address apparently

	if(bind(host_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) == -1){
		logger(TG_SOCK, "Binding socket", LG_ERR | LG_FTL);
	}

	if(listen(host_fd, 5) == -1) { logger(TG_SOCK, "Starting listening", LG_ERR | LG_FTL); }

/*	Main Loop	*/
	while(1){
		// Accept connection
		socklen_t addrlen = sizeof(struct sockaddr_in);
		client_fd = accept(host_fd, (struct sockaddr *)&client_addr, &addrlen);
		if(client_fd == -1) { logger(TG_SOCK, "Error accepting connection", LG_ERR); }

		sprintf(tempstring, "New connection from %s:%d", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		logger(TG_DBG, tempstring, 0);

		// Upgrade to TLS
		if(tls_accept_socket(server_tls, &client_tls, client_fd) == -1){
			logger(TG_TLS, "Error upgrading to tls", 0);
		}
			
		// Handle the connection
		handle_connection(client_tls, &client_addr);
		
		tls_close(client_tls);
		tls_free(client_tls);

		close(client_fd);

		logger(TG_DBG, "Closed connection", 0);
	}
	tls_close(server_tls);
	tls_free(server_tls);
	close(host_fd);

	return 0;
}

int handle_connection(struct tls *client_tls, struct sockaddr_in *client_addr){
	int file_fd, found_cr = 0;
	ssize_t read_len;
	char *current_char;
	char buffer[1024], file_buffer[1024], path[1024], tempstring[2048];	//TODO remove the need for tempstring (own implementation ish of ...?)
//TODO could use buffer for file_buffer, check once done query

/*	Get the request	*/
	current_char = buffer;
//TODO Log
//TODO remove nonprintable characters
	while(1){
		// Read & make sure something is read
		if(tls_read(client_tls, current_char, 1) == 0){
			return -1;
		}
		// No premature ends of the request
		else if(*current_char == '\0'){
			return -1;
		}
		// Terminate the string & stop recieving after CRLF
		else if(*current_char == '\n' && found_cr == 1){
			*(--current_char) = '\0';
			break;
		}
		else if(*current_char == '\r'){
			found_cr = 1;
		}

//TODO Could be off by one
		// Make sure there are no buffer overflows	
		if(current_char-buffer == 1023){
			return -1;
		} else{
			current_char++;
		}
	}
	sprintf(tempstring, "Request got from %s:%d: %s", \
		inet_ntoa(client_addr->sin_addr), ntohs(client_addr->sin_port), buffer);
	logger(TG_CONN, tempstring, 0);
	
/*	Check the protocol is actually gemini	*/
	if(strncmp(buffer, "gemini", 6)){
		logger(TG_CONN, "Protocol is not gemini", 0);
		return -1;
	}
	
/*	Check whether the hostname matches	*/
	//TODO implement this
//	strtok(buffer, "/");
//	current_char = strtok(NULL, "/");
	// current_char is hostname

/*	Split the path, query & fragment	*/
	strcpy(path, WEBROOT);

	// Start the path after the host
	current_char = strchr(strchr(buffer, '/')+2, '/');
	if(current_char == NULL){
		strcat(path, "/");
	} else{
		strcat(path, current_char);
	}
	
	// Crop off the query and fragment if present
	current_char = strpbrk(path, "#?");	//IS QUERY (an only with ?) (should probs use buffer tho)
	if(current_char != NULL){
		path[current_char-path] = '\0';
	}

	// Make sure it doesnt go backwards in the file tree
	if(strstr(path, "/../") != NULL){
		send_header(59, "Request contains bad file path", client_tls); 
		return -1;
	}
	
	// Add index.gmi if necessary
	if(path[strlen(path)-1] == '/'){
		strcat(path, "index.gmi");
	}

	logger(TG_DBG, path, 0);

/*	Process the query	*/
//TODO do something with the query (included with expanding return codes i guess?)

/*	Open, read and send the appropriate file	*/
	// Open the file
	file_fd = open(path, O_RDONLY);	
	if(file_fd == -1){
		logger(TG_FD, "Error getting requested file descriptor", LG_ERR);
		if(errno==ENOENT){ //TODO add other error codes to here (like perms) (maybe)
			return send_header(51, "File not found", client_tls);
		} else{
			return send_header(41, "The server experiencend an error finding the requested file", client_tls);
		}
	}
	
	// Read the file and immediately send it
	read_len = read(file_fd, file_buffer, 1024);
	if(read_len == -1){
		logger(TG_FD, "Error reading from file", LG_ERR);
		if(errno == EISDIR){
			return send_header(51, "File not found", client_tls);
		}
		return send_header(41, "The server experiencend an error finding the requested file", client_tls);
	}	
	if(send_header(20, "text/gemini", client_tls) == -1){ return -1; }
	
	while(read_len > 0){
		if(tls_write(client_tls, file_buffer, read_len) == -1){
			logger(TG_MSG, "Error sending message body", 0);
		}
		read_len = read(file_fd, file_buffer, 1024);
		if(read_len == -1){
			logger(TG_FD, "Error reading from file", LG_ERR);
		}
	}

	close(file_fd);


	return 0;
}

int send_header(const int status, const char* meta, struct tls *client_tls){
	char header[1029];	//status(2)+space(1)+meta(1024)+crlf(2)
	sprintf(header, "%d %.1024s\r\n", status, meta);

	logger(TG_HDR, header, 0);
	
	if(tls_write(client_tls, header, strlen(header)) == -1){
		logger(TG_MSG, "Error sending header", 0);
		return -1;
	}
	return 0;
}

void dump(const char* string, const size_t len){
	long unsigned int i, char_no;
	for(char_no = 0; char_no < len; char_no++){
		printf("%02x ", string[char_no]);
		if(((char_no%16)==15) || (char_no==len-1)){
			for(i = 0; i < 15-(char_no%16); i++){
				printf("   ");
			}
			printf("| ");
			for(i = char_no-(char_no%16); i<=char_no; i++){
				if((31 < string[i]) && (string[i] < 127)){
					printf("%c", string[i]);
				} else{
					printf(".");
				}
			}
			printf("\n");
		}
	}
}
