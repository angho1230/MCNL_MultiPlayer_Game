#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <time.h>
#include "socket.h"
#include "game.h"


int * player_sock;
int game_on = 1;

typedef struct{
    game_info * ginfo;
    board_info * binfo;
    int id;
}arg;

int write_game_info(int sock, game_info * ginfo, board_info * binfo){
    if(ginfo == 0x0){
        printf("Unable to find game info\n");
    }
    write(sock, ginfo->board, binfo->room_width*binfo->room_height);
    write(sock, ginfo->players, sizeof(player)*binfo->player_number);
    return 0;
}

int game_init(board_info * binfo, game_info * ginfo, int n, int s, int b, int t, const char * p){
    if(ginfo == 0x0){
        printf("Unable to find game info\n");
        return -1;
    }
    if(n <= 1){
        printf("The Number of player should be more than 1\n");
        return -1;
    }
    if(n % 2 == 1){
        printf("The number of player should be even\n");
        return -1;
    }
    if(s < 10){
        printf("Size of the board should be bigger than or equal to 10\n");
        return -1;
    }
    if(s > 200){
        printf("Size of the board should be less than or equal to 200\n");
        return -1;
    }
    if(b <= s){
        printf("The number of tile should be more than %d\n", s);
        return -1;
    }
    if(b >= s*s){
        printf("The number of tile should be less than %d\n", s*s);
        return -1;
    }
    if(b % 2 == 1){
        printf("The number of tile should be even\n");
        return -1;
    }
    if(t <= 10){
        printf("Time should be bigger than 10\n");
        return -1;
    }
    if(t > 300){
        printf("Time should not exceed 5 miniutes\n");
        return -1;
    }
    int sock = tcp_server_create(p);
    ginfo->board = (char *)malloc(sizeof(char)*s*s); 
    memset(ginfo->board, 0, s*s*sizeof(char));
    
    srand(time(0x0));
    int tsize = s*s;
    for(int i = 0; i < b; i++){
        int loc = rand()%tsize;
        if(ginfo->board[loc]!=0){
            i--;
            continue;
        }
        ginfo->board[loc] = i%2 + 1;
    }
    ginfo->players = (player *)malloc(sizeof(player)*n);
    for(int i = 0; i < n; i++){
        ginfo->players[i].player_id = i;
        ginfo->players[i].team = i%2;
        ginfo->players[i].x = i%2 ? s-1 : 0;
        ginfo->players[i].y = i%2;
    }
    binfo->tile_num = b;
    binfo->play_time = t;
    binfo->room_width = s;
    binfo->room_height = s;
    return sock;
}


int game_start(board_info * binfo, game_info * ginfo, int serv_sd){
    int epfd, event_cnt;
    int players=0;
    struct epoll_event *ep_events;
    struct epoll_event event;
    
    player_sock = (int *)malloc(sizeof(int)*binfo->player_number);
    for(int i = 0; i < binfo->player_number; i++){
        player_sock[i] = -1;
    }

    epfd = epoll_create(binfo->player_number);
    ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event)*binfo->player_number);
    event.events = EPOLLIN;
    event.data.fd = serv_sd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sd, &event);
    
    while(players < binfo->player_number){
        event_cnt = epoll_wait(epfd, ep_events, binfo->player_number, -1);
        if(event_cnt == -1){
            printf("Error occurred in epoll wait()\n");
            return -1;
        }
        for(int i = 0; i < event_cnt; i++){
            if(ep_events[i].data.fd == serv_sd){
                int clnt_sd = tcp_accept(serv_sd);
                event.events = EPOLLIN;
                event.data.fd = clnt_sd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sd, &event);
                player_sock[players] = clnt_sd;
                printf("connected client: %d\n", clnt_sd);
                for(int j = 0; j < binfo->player_number; j++){
                    if(player_sock[j] == -1){
                        player_sock[j] = clnt_sd;
                        players++;
                        break;
                    }
                }
            }
            else{
                char buf[10];
                int len = read(ep_events[i].data.fd, buf, 10);
                if(len == 0){
                    epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, 0x0);
                    close(ep_events[i].data.fd);
                    printf("closed client %d\n", ep_events[i].data.fd);
                    for(int j = 0; j < binfo->player_number; j++){
                        if(player_sock[j] == ep_events[i].data.fd){
                            player_sock[j] = -1;
                            players--;
                            break;
                        }
                    }
                }
            }
        }
    }
    printf("Starting Game\n");
    for(int i = 0; i < binfo->player_number; i++){
        pthread_t pid;
        arg a;
        a.id = i;
        a.ginfo = ginfo;
        binfo->id = i;
        a.binfo = binfo;
        write(player_sock[i], binfo, sizeof(board_info));
        pthread_create(&pid, 0x0, game_clnt, &a);    
    }
}

void * game_clnt(void * _a){
    int id = ((arg *)_a)->id;
    game_info * ginfo = ((arg *)_a)->ginfo;  
    int sock = player_sock[id];
    board_info * binfo = ((arg *)_a)->binfo;
    write_game_info(sock, ginfo, binfo);
    while(game_on){
        read_clnt;
    }
    return 0x0;
}

