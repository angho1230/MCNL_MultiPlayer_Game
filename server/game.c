#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include "socket.h"
#include "game.h"

static int * player_sock;
static game_info * ginfo;
static board_info * binfo;
static int game_on;
static pthread_mutex_t mutex;

static int flag = 0;

typedef struct{
    int id;
}arg;

/***
* read game info 
* pair with write_game_info
*/
int read_game_info(int sock){
    if(ginfo == 0x0){
        printf("Unable to find game info\n");
        return -1;
    }
    read(sock, ginfo->board, binfo->room_width*binfo->room_height);
    read(sock, ginfo->players, sizeof(player)*binfo->player_number);
    read(sock, &ginfo->game_end, sizeof(int));
    return 0;
}
/*
* write game info
* pair with read_game_info
*/
int write_game_info(int sock){
    if(ginfo == 0x0){
        printf("Unable to find game info\n");
        return -1;
    }
    write(sock, ginfo->board, binfo->room_width*binfo->room_height);
    write(sock, ginfo->players, sizeof(player)*binfo->player_number);
    write(sock, &ginfo->game_end, sizeof(int));
    return 0;
}
/*
* initialize game_info and board_info with parsed option
*/
int game_init(board_info * binfo_l, game_info * ginfo_l, int n, int s, int b, int t, const char * p){
    if(ginfo_l == 0x0){
        printf("Unable to find game info\n");
        return -1;
    }
    if(binfo_l == 0x0){
        printf("Unable to find board info\n");
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
    ginfo = ginfo_l;
    binfo = binfo_l;
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
    ginfo->game_end = 0;
    ginfo->players = (player *)malloc(sizeof(player)*n);
    for(int i = 0; i < n; i++){
        ginfo->players[i].player_id = i;
        ginfo->players[i].team = i%2+1;
        ginfo->players[i].x = i%2 ? s-1 : 0;
        ginfo->players[i].y = i%2;
    }
    
    binfo->tile_num = b;
    binfo->play_time = t;
    binfo->room_width = s;
    binfo->room_height = s;
    binfo->player_number = n;
    pthread_mutex_init(&mutex, 0x0);

    game_on = 1000/MILSEC * binfo->play_time;
    return sock;
}

/*
* start and run the entire game
*/
int game_start(int serv_sd){
    int epfd, event_cnt;
    int players=0;
    struct epoll_event *ep_events;
    struct epoll_event event;
    struct itimerval timer;
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = (MILSEC*1000);
    timer.it_interval = timer.it_value;

    //player_sock to store the socket descriptor
    player_sock = (int *)malloc(sizeof(int)*binfo->player_number);
    for(int i = 0; i < binfo->player_number; i++){
        player_sock[i] = -1;
    }
    epfd = epoll_create(binfo->player_number);
    if(epfd == -1){
        int err = errno;
        printf("failed creating epoll\n");
        switch(err){
            case  EINVAL : 
                printf("size is not positive.\n");
                break;

            case EMFILE:
                printf("Limit of # of opend files\n");
                break;


            case ENOMEM :
                printf("No sufficient memory"); 
                break;
            }
        return -1;
    }
    ep_events = malloc(sizeof(struct epoll_event)*binfo->player_number);
    event.events = EPOLLIN;
    event.data.fd = serv_sd;
    epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sd, &event);
    
    //use epoll to track the user connection to the server socket
    while(players < binfo->player_number){
        printf("searching for players(#%d)\n", binfo->player_number - players);
        event_cnt = epoll_wait(epfd, ep_events, binfo->player_number, -1);
        if(event_cnt == -1){
            int err = errno;
            printf("Error occurred in epoll wait()\n");
            switch(err){
                case EBADF : 
                    printf("epfd is not a valid file descriptor.\n");
                    break;
                case EFAULT :
                    printf("The memory area pointed to by events is not accessible with write permissions.\n");
                    break;
                case EINTR :
                    printf("The call was interrupted by a signal handler before either\
              (1) any of the requested events occurred or (2) the\
              timeout expired; see signal(7).\n"); 
                    break;
                case EINVAL: 
                    printf("epfd is not an epoll file descriptor, or maxevents is less\
              than or equal to zero.\n"); 
                    break;
            }
            return -1;
        }
        for(int i = 0; i < event_cnt; i++){
            //new connection
            if(ep_events[i].data.fd == serv_sd){
                int clnt_sd = tcp_accept(serv_sd);
                event.events = EPOLLIN;
                event.data.fd = clnt_sd;
                epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sd, &event);
                printf("connected client: %d\n", clnt_sd);
                for(int j = 0; j < binfo->player_number; j++){
                    if(player_sock[j] == -1){
                        player_sock[j] = clnt_sd;
                        players++;
                        printf("added player #%d\n", j);
                        break;
                    }
                }
            }
            //lost connection
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
    //write board_info to every client
    for(int i = 0; i < binfo->player_number; i++){
        binfo->id = i;
        write(player_sock[i], binfo, sizeof(board_info));
    }

    //make thread to track client input
    for(long i = 0; i < binfo->player_number; i++){
        pthread_t pid;
        pthread_create(&pid, 0x0, game_clnt, (void *)i);
    }

    //set timer in 100ms rate
    if(signal(SIGALRM, game_timer) == SIG_ERR){
        printf("Error occurred in signal()\n");
        return -1;
    }
    if(setitimer(ITIMER_REAL, &timer, 0x0) == -1){
        printf("Error occurred in setitimer()\n");
        return -1;
    }
    while(game_on != 0){
        pause();
    }
    timer.it_value.tv_usec = 0;
    timer.it_value.tv_sec = 0;
    timer.it_interval = timer.it_value;
    if(setitimer(ITIMER_REAL, &timer, 0x0) == -1){
        printf("Error occurred in setitimer() (timer delete)\n");
        return -1;
    }
    printf("Game Over\n");
    return 0;
}


/*
* for every 100ms, check if flag is true
* if true write new game_info to clients
*/
void game_timer(int a){
    game_on--;
    if(game_on == 0){
        ginfo->game_end = 1;
    }
    if(game_on != 0 && flag == 0 && game_on%MILSEC != 0){
        return;
    }
    flag = 0;
    for(int i = 0; i < binfo->player_number; i++){
        write_game_info(player_sock[i]);
    }
    printf("writing game info %d\n", game_on);
    return;
}

/*
* check if the movement is valid if it is not, return 0
* use mutex to avoid race condition
*/
int game_check_valid(int input, int id){
    if(ginfo == 0x0){
        printf("Unable to find game info\n");
        return 0;
    }
    if(binfo == 0x0){
        printf("Unable to find board info\n");
        return 0;
    }
    int ret = 1;
    pthread_mutex_lock(&mutex);
    switch(input){
        case 'w': {
            if(ginfo->players[id].y > 0) {
                for(int i = 0; i < binfo->player_number; i++){
                    if(i != id && ginfo->players[i].x == ginfo->players[id].x && ginfo->players[i].y == ginfo->players[id].y-1){
                        ret = 0;
                        break;
                    }
                }
                if(ret) ginfo->players[id].y--;
            }
        }
        break;
        case 's': {
            if(ginfo->players[id].y < binfo->room_height-1) {
                for(int i = 0; i < binfo->player_number; i++){
                    if(i != id && ginfo->players[i].x == ginfo->players[id].x && ginfo->players[i].y == ginfo->players[id].y+1){
                        ret = 0;
                        break;
                    }
                }
                if(ret) ginfo->players[id].y++;
            }
        }
        break;
        case 'a': {
            if(ginfo->players[id].x > 0) {
                for(int i = 0; i < binfo->player_number; i++){
                    if(i != id && ginfo->players[i].x == ginfo->players[id].x-1 && ginfo->players[i].y == ginfo->players[id].y){
                        ret = 0;
                        break;
                    }
                }
                if(ret) ginfo->players[id].x--;
            }
        }
        break;
        case 'd': {
            if(ginfo->players[id].x < binfo->room_width-1) {
                for(int i = 0; i < binfo->player_number; i++){
                    if(i != id && ginfo->players[i].x == ginfo->players[id].x+1 && ginfo->players[i].y == ginfo->players[id].y){
                        ret = 0;
                        break;
                    }
                }
                if(ret) ginfo->players[id].x++;
            }
        }
        break;
        case ' ':{
            int x = ginfo->players[id].x;
            int y = ginfo->players[id].y;
            if(ginfo->board[y*binfo->room_width+x] == 0){
                ret = 0;
            }
            else{
                ginfo->board[y*binfo->room_width+x] = ginfo->players[id].team;
            }
        }
        break;
        default: break;
    }
    if(ret) flag ++;
    pthread_mutex_unlock(&mutex);
    return ret;
}

/*
* function to handle client
*/
void * game_clnt(void * _a){
    int id = (long)_a;
    int sock = player_sock[id];
    printf("id : %d\n", id);
    while(game_on){
        int input;
        read(sock, &input, sizeof(int));
        game_check_valid(input, id);
    }
    return 0x0;
}

