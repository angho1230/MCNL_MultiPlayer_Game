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
#include "socket.h"
#include "game.h"

void * clnt_handle(void * arg);

int main(int argc, char * argv[]){
    int opt;
    int n = 0, s = 0, b = 0, t = 0, p = 0;
    while((opt = getopt(argc, argv, "n:s:b:t:p:")) != -1){
        switch(opt){
            case 'n': n = atoi(optarg); break;
            case 's': s = atoi(optarg); break;
            case 'b': b = atoi(optarg); break;
            case 't': t = atoi(optarg); break;
            case 'p': p = atoi(optarg); break;
            case '?':
                printf("option %c is not supported\n", optopt);
                exit(1);
        }
    }
    game_info ginfo;
    board_info binfo;
    int epoll_size;
    int sock;
    if((sock = game_init(&binfo, &ginfo,n,s,b,t,p)) == -1){
        printf("Terminating Program\n");
        exit(1);
    }
     game_wait(&binfo, &ginfo, )
}

void * clnt_handle(void * arg){
}
