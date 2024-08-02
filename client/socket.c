#include <stdio.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "socket.h"


int tcp_server_create(const char * port_c){
    int serv_sd=socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_adr;

    if(serv_sd == -1){
        error_handling("socket() error");
    }

    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;
    serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_adr.sin_port = htons(atoi(port_c));

    if(bind(serv_sd, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("bind() error");
    }
    
    if(listen(serv_sd, 5) == -1){
        error_handling("listen() error");
    }
    return serv_sd;
}

int tcp_client_create(const char * port_c, const char * server_ip, struct sockaddr_in * serv_adr){
    int sock=socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        error_handling("socket() error");
    }
    memset(serv_adr, 0, sizeof(struct sockaddr_in));
    serv_adr->sin_family = AF_INET;
    serv_adr->sin_addr.s_addr = inet_addr(server_ip);
    serv_adr->sin_port = htons(atoi(port_c));
    return sock;
}

int read_full(int sd, void * buf, size_t nbytes){
    if(buf == 0x0) return -1;
    if(sd< 0) return -1;
    int read_size = 0;
    while(read_size < nbytes){
        read_size += read(sd, buf+read_size, nbytes);
    }
    return read_size;
}

int write_v(int sd, void * buf, size_t n, size_t size){
    if(buf == 0x0) return -1;
    if(sd< 0) return -1;
    for(int i = 0; i < n; i++){
        write(sd, buf+i*size, size);
    }
    return 1;
}

void error_handling(char* message){
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
