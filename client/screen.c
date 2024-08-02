#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ncurses.h>
#include "screen.h"

void print_screen(const char *buf, const char ** result, int search_count){
    clear();
    mvprintw(0, 0, ">: ");
    mvprintw(0, 4, buf);
    for(int i = 0; i < search_count; i++){
        mvprintw(i+1, 0, result[i]);
    }
    refresh();
}
