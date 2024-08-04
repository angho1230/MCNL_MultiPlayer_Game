#include <stdio.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
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



int game_on = 1;
static board_info binfo;
static game_info ginfo;

int main(int argc, char * argv[]){
    int sock;
   
    
    int s = 20;
    int n = 10;
    int b = 100;
    int t = 10;
    

    ginfo.board = (char *)malloc(sizeof(char)*s*s); 
    memset(ginfo.board, 0, s*s*sizeof(char));
    
    srand(time(0x0));
    int tsize = s*s;
    for(int i = 0; i < b; i++){
        int loc = rand()%tsize;
        if(ginfo.board[loc]!=0){
            i--;
            continue;
        }
        ginfo.board[loc] = i%2 + 1;
    }
    ginfo.game_end = 0;
    ginfo.players = (player *)malloc(sizeof(player)*n);
    for(int i = 0; i < n; i++){
        ginfo.players[i].player_id = i;
        ginfo.players[i].team = i%2;
        ginfo.players[i].x = i%2 ? s-1 : 0;
        ginfo.players[i].y = i%2;
    }
    binfo.tile_num = b;
    binfo.play_time = t;
    binfo.room_width = s;
    binfo.room_height = s;
    binfo.id = 0;
    init_print(&binfo);
    print_game_info(&ginfo);
    getch();
    end_print();
    return 0;
}
