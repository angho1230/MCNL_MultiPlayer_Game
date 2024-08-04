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
#include "print.h"



void * read_from_server(void * a);
int game_on = 1;
static board_info binfo;
static game_info ginfo;

int main(int argc, char * argv[]){
    int sock;
    struct sockaddr_in serv_adr;

    if(argc != 3){
        printf("Usage: %s <server_ip> <port>\n", argv[0]);
        return -1;
    }
    sock = tcp_client_create(argv[2], argv[1], &serv_adr);
    if(sock == -1){
        printf("Unable to create server socket\n");
        return -1;
    }
    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("connect() error!");
    }
    read(sock, &binfo, sizeof(board_info));
    init_print(&binfo);
    pthread_t pid;
    pthread_create(&pid, 0x0, read_from_server, &sock);
    while(game_on){
        int c = getch();
        switch(c){
            case 'w':
            case 'a':
            case 's':
            case 'd':
            case ' ':
            write(sock, &c, sizeof(int));
            break;
            default:
            break;
        }
    }
    return 0;
}

void * read_from_server(void * a){
    int sock = *(int *)a;
    read_game_info(sock, &ginfo);
    print_game_info(&ginfo);
    if(ginfo.game_end){
        game_on = 0;
    }
    return 0x0;
}
