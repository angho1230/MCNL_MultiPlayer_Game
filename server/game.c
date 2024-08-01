#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "socket.h"
#include "game.h"

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
