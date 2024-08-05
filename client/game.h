#include "board.h"
#define MILSEC 100

typedef struct _game_info{
    char* board;
    player* players;
    int game_end;
}game_info;

int game_check_valid(int input, int id);
int read_game_info(int sock, game_info * ginfo);
int write_game_info(int sock);
int game_init(board_info * binfo, game_info * ginfo, int n, int s, int b, int t, const char * p);
void * game_clnt(void * );
int game_wait();
int game_start(int serv_sd);
void game_timer(int);
int game_info_init(game_info *ginfo, board_info *binfo);
