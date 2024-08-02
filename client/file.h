#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#define BUF_SIZE 1024

typedef struct file{
    char name[256];
    off_t size;
    mode_t auth;
    unsigned char type;
} file;

int get_files(const char * path, file ** files);
int read_to_file(int fd, const char * path, file finfo);
int write_from_file(int fd, const char * path, file finfo);

void print_files(file * files, int fcount);  
int get_file(const char * path, file * dstf, const char * fname);
