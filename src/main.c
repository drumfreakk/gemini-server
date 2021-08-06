#include <stdio.h>
#include <tls.h>
#include <unistd.h>		/* close() */
#include <sys/socket.h>	/* SOCKETS (socket(), bind()...) */
#include <netinet/ip.h>	/* IPv4 () */
#include <arpa/inet.h>	/* network byte order */

#include "tools.h"


int main(){
//	printf("%i\n", tls_init());
	int host_fd, client_fd, recv_len, yes = 1;
	struct sockaddr_in local_addr, client_addr;
	char buffer[1024];

	host_fd = socket(AF_INET, SOCK_STREAM, 0);

	if(setsockopt(host_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		fatal("Error setting socket options");

	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(1965);
	local_addr.sin_addr.s_addr = 0;	// Automatically sets to local address apparently

	if(bind(host_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr)) == -1)
		fatal("Error binding");

	if(listen(host_fd, 5) == -1)
		fatal("Error listening");
	
	while(1){
		socklen_t addrlen = sizeof(struct sockaddr_in);
		client_fd = accept(host_fd, (struct sockaddr*)&client_addr, &addrlen);
		if(client_fd == -1)
			fatal("Error accepting connection");

		printf("New connection from %s:%d\n", \
			inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		send(client_fd, "Hello, world!\n", 14, 0);
		
		recv_len = recv(client_fd, &buffer, 1024, 0);
		while(recv_len > 0){
			printf("Recieved %d chars\n", recv_len);
			dump(buffer, recv_len);
			recv_len = recv(client_fd, &buffer, 1024, 0);
		}

		close(client_fd);
	}

	close(host_fd);
	return 0;
}


