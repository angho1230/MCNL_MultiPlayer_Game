#include <stdio.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <ncurses.h>
#include "socket.h"
#include "game.h"

int main(int argc, char * argv[]){
    if(argc != 3){
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return -1;
    }
    struct sockaddr_in serv_adr;
    int sock = tcp_client_create(argv[2], argv[1], &serv_adr);
    if(sock == -1){
        printf("Unable to create server socket\n");
        return -1;
    }
    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("connect() error!");
    }
    initscr();
    return 0;

}
