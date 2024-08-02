#include <stdio.h>
#include <netinet/in.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/socket.h>
#include <ncurses.h>
#include "socket.h"
#include "screen.h"

#define BUF_SIZE 1024

int main(int argc, char * argv[]){
    if(argc != 3){
        printf("Usage %s <sever ip> <port>\n", argv[0]);
        exit(1);
    }
    struct sockaddr_in serv_adr;
    int sock = tcp_client_create(argv[2], argv[1], &serv_adr);
    if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1){
        error_handling("connect() error!");
    }
    static struct termios t, oldt;
    tcgetattr( STDIN_FILENO, &t);
    oldt = t;
    t.c_lflag &= ~(ICANON | ECHO);
    tcsetattr( STDIN_FILENO, TCSANOW, &t);

    char buf[BUF_SIZE];
    int len = 0;

    initscr();
    while(1){
        int c = getchar();
        switch(c){
            case '\n':
                continue;
            case 127:
            case 8:
                if(len > 0)len--;
                break;
            default:
                buf[len++] = c;
                break;
        }
        buf[len] = '\0';
        //printf("> %s\n", buf);
        if(len == 0){print_screen(buf, 0, 0); continue;}
        write(sock, buf, len);
        int search_count;
        read(sock, &search_count, sizeof(int));
        char ** result = (char**)malloc(search_count*sizeof(char*));
        for(int i = 0; i < search_count; i++){
            int strl;
            read(sock, &strl, sizeof(int));
            result[i] = (char*)malloc(sizeof(char)*strl+1);
            read_full(sock, result[i], strl);
            result[i][strl] = '\0';
        }
        print_screen(buf, result, search_count);
        for(int i = 0; i < search_count; i++){
            free(result[i]);
        }
        free(result);
    }
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
    endwin();
}
