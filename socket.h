#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


int setup_new_socket(int mode, char * server_ip){

    //server
    if(mode){
        int server_fd, new_socket, valread;
        struct sockaddr_in address;
        int opt = 1;
        int addrlen = sizeof(address);

        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
            perror("socket failed\n");
            exit(EXIT_FAILURE);
        }

        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);
    
        if(bind(server_fd, (struct sockaddr *) &address, sizeof(address))<0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }

        if(listen(server_fd, 3) < 0){
            perror("listen");
            exit(EXIT_FAILURE);
        }

        if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*) &addrlen))<0){
            perror("Accept");
            exit(EXIT_FAILURE);
        }

        return new_socket;
    }

    else {

		struct addrinfo hints, *res;
		int sockfd;
		
		// first, load up address structs with getaddrinfo():
		
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		
		getaddrinfo(server_ip, "8080", &hints, &res);
		
		// make a socket:
		
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		
		// connect!
		
		connect(sockfd, res->ai_addr, res->ai_addrlen);
		freeaddrinfo(res);
		return sockfd;
    }
}

