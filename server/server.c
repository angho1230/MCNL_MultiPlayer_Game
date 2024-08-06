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


/***
*  Parse an option and initialize game
*  After that start a game
* ***/
int main(int argc, char * argv[]){
    int opt;
    char * p;
    int n = 0, s = 0, b = 0, t = 0;
    while((opt = getopt(argc, argv, "n:s:b:t:p:")) != -1){
        switch(opt){
            case 'n': n = atoi(optarg); break;
            case 's': s = atoi(optarg); break;
            case 'b': b = atoi(optarg); break;
            case 't': t = atoi(optarg); break;
            case 'p': p = optarg; break;
            case '?':
                printf("option %c is not supported\n", optopt);
                exit(1);
        }
    }
    if(!(n && s && b && t && p)){
        printf("Usage ./server -n <# of players> -s <size of field> -b <# of tiles> -t <time> -p <port>");
        exit(1);
    }
    printf("%d %d %d %d %s\n", n, s, b, t, p);
    game_info ginfo;
    board_info binfo;
    int serv_sd;
    if((serv_sd = game_init(&binfo, &ginfo,n,s,b,t,p)) == -1){
        printf("Terminating Program\n");
        exit(1);
    }
    if(game_start(serv_sd) == -1){
        printf("Terminating Program\n");
        exit(1);
    }
    return 0;
}
