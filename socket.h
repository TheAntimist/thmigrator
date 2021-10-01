#include <unistd.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#define PORT 8080


int setup_new_socket(int mode, char * server_ip){

    //client
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

    else{

        int sock = 0;
        struct sockaddr_in serv_addr;
        
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0){
            printf("\n Socket creation error \n");
            return -1;
        }
        
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        if(inet_pton(AF_INET, server_ip, &serv_addr.sin_addr)<=0){
            printf("\n Invalid address\n");
            return -1;
        }
    
        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
            printf("\nConnection failed\n");
            return -1;
        }        
        
        return sock;
    }   

}
