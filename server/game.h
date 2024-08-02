#include "board.h"

typedef struct _game_info{
    char* board;
    player* players;
}game_info;

int write_game_info(int sock, game_info * ginfo, board_info * binfo);
int game_init(board_info * binfo, game_info * ginfo, int n, int s, int b, int t, const char * p);
void * game_clnt(void * );
int game_wait(board_info * binfo, game_info * ginfo);
int game_start(board_info * binfo, game_info * ginfo, int serv_sd);

