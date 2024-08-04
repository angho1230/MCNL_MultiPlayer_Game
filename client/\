#include <ncurses.h>
#include "print.h"

static game_info * ginfo;
static board_info * binfo;
int mx, my;

void rectangle(int y1, int x1, int y2, int x2)
{
    mvhline(y1, x1, 0, x2-x1);
    mvhline(y2, x1, 0, x2-x1);
    mvvline(y1, x1, 0, y2-y1);
    mvvline(y1, x2, 0, y2-y1);
    mvaddch(y1, x1, ACS_ULCORNER);
    mvaddch(y2, x1, ACS_LLCORNER);
    mvaddch(y1, x2, ACS_URCORNER);
    mvaddch(y2, x2, ACS_LRCORNER);
}

void tile(int x, int y){
    rectangle(y, x, y+RECT_H, x+RECT_W);
}

int idx_to_xy(int ix, int iy, int * x, int * y){
    if(binfo == 0x0 || ginfo == 0x0) return -1;
    int px = ginfo->players[binfo->id].x;
    int py = ginfo->players[binfo->id].y;
    getmaxyx(stdscr, my, mx);
    int prefixx = 2;
    int prefixy = 6;
    int max_ix = (mx - prefixx)/(RECT_W+1)/2;
    int max_iy = (my - prefixy)/(RECT_H+1)/2;
    if(max_ix + px < ix || px - max_ix > ix || max_iy + py < iy || py - max_iy > iy){
        return -1;
    }
    *x = prefixx + (max_ix + ix - px)*(RECT_W + 1);
    *y = prefixy + (max_iy + iy - py)*(RECT_H + 1);
    return 0;
}

void print_game_info(game_info * _ginfo){
    ginfo = _ginfo;
    getmaxyx(stdscr, my, mx);
    for(int i = 0; i < binfo->room_width*binfo->room_height; i++){
        int x, y;
        if(idx_to_xy(i%binfo->room_width, i/binfo->room_width, &x, &y) == 0){
            tile(x, y);
        }
    }
    for(int i = 0; i < binfo->player_number; i++){
        int x, y;
        if(idx_to_xy(ginfo->players[i].x, ginfo->players[i].y, &x, &y) == 0){
            x += 1;
            y += 1;
            printw("%d", i);
        }
    }
}

int init_print(board_info *_binfo){
    binfo = _binfo;
    ginfo = 0x0;
    initscr();
    start_color();
    return 0;
}
