#include <ncurses.h>
#include "print.h"

static game_info * ginfo;
static board_info * binfo;
int mx, my;
WINDOW * win, *win_prev;

void rectangle(int y1, int x1, int y2, int x2)
{
    mvwhline(win, y1, x1, 0, x2-x1);
    mvwhline(win, y2, x1, 0, x2-x1);
    mvwvline(win, y1, x1, 0, y2-y1);
    mvwvline(win, y1, x2, 0, y2-y1);
    mvwaddch(win, y1, x1, ACS_ULCORNER);
    mvwaddch(win, y2, x1, ACS_LLCORNER);
    mvwaddch(win, y1, x2, ACS_URCORNER);
    mvwaddch(win, y2, x2, ACS_LRCORNER);
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
    win_prev = win;
    ginfo = _ginfo;
    getmaxyx(stdscr, my, mx);
    win = newwin(my, mx, 0, 0);
    for(int i = 0; i < binfo->room_width*binfo->room_height; i++){
        int x, y;
        if(idx_to_xy(i%binfo->room_width, i/binfo->room_width, &x, &y) == 0){
            if(ginfo->board[i] == 0)
                tile(x, y);
            else if(ginfo->board[i] == 1){
                wattron(win, COLOR_PAIR(1));
                tile(x, y);
                wattroff(win, COLOR_PAIR(1));
            }
            else if(ginfo->board[i] == 2){
                wattron(win, COLOR_PAIR(2));
                tile(x, y);
                wattroff(win, COLOR_PAIR(2));
            }
        }
    }
    for(int i = 0; i < binfo->player_number; i++){
        int x, y;
        if(idx_to_xy(ginfo->players[i].x, ginfo->players[i].y, &x, &y) == 0){
            x += 1;
            y += 1;
            mvwprintw(win, y, x, "%d", i);
        }
    }
    wrefresh(win);
    delwin(win_prev);
}

int init_print(board_info *_binfo){
    binfo = _binfo;
    ginfo = 0x0;
    initscr();
    start_color();
    noecho();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    return 0;
}
