#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <dirent.h>

#include "file.h"
#include "socket.h"

int get_files(const char * path, file ** files){
    DIR * d;
    struct dirent* dirst;
    struct stat stst;
    int count = 0;
    int fsize = 16;

    if ((d = opendir(path)) == 0x0){
        printf("Failed opening %s\n", path);
        return -1;
    }
    file * new_files = (file*)malloc(fsize*sizeof(file));
    while((dirst = readdir(d)) != 0x0){
        strcpy(new_files[count].name, dirst->d_name);
        char * f_path = (char*)malloc(sizeof(char)*(strlen(dirst->d_name)+strlen(path)+2));
        sprintf(f_path, "%s/%s", path, dirst->d_name);
        if(stat(f_path, &stst) == -1){
            printf("Failed to stat %s\n", f_path);
            free(f_path);
            continue;
        }
        new_files[count].auth = stst.st_mode;
        new_files[count].size = stst.st_size;
        new_files[count].type = dirst->d_type;
        count++;
        if(count >= fsize-1){
            file * temp = (file*)malloc(fsize*2*sizeof(file));
            memcpy(temp, new_files, fsize*sizeof(file));
            fsize *= 2;
            free(new_files);
            new_files = temp;
        }
        free(f_path);
    }
    *files = new_files;
    return count;
}

int get_file(const char * path, file * dstf, const char * fname){
    char * f_path = (char *)malloc(sizeof(char) * (strlen(path) + strlen(fname) +2));
    sprintf(f_path, "%s/%s", path, fname);
    struct stat stst;
    if(stat(f_path, &stst) == -1){
        printf("Failed to stat %s\n", f_path);
        free(f_path);
        return -1;
    }
    dstf->auth = stst.st_mode;
    dstf->size = stst.st_size;
    strcpy(dstf->name, fname);
    free(f_path);
    return 0;
}

int read_to_file(int fd, const char * path, file finfo){
    int size = finfo.size;
    char * fname = (char *)malloc(sizeof(char) * (strlen(path)+strlen(finfo.name) + 2));
    sprintf(fname, "%s/%s", path, finfo.name);
    FILE * fp = fopen(fname, "wb");
    if(fp == 0x0){
        printf("cannot open %s\n", path);
        return -1;
    }
    int read_size = 0, read_len;
    int buf[BUF_SIZE];
    while(size > read_size){
        read_len = read(fd, buf, BUF_SIZE);
        if(read_len == 0){
            return -1;
        }
        read_size += read_len;
        fwrite(buf, sizeof(char), read_len, fp);
    }
    chmod(fname, finfo.auth);
    free(fname);
    fclose(fp);
    return 0;
}

int write_from_file(int fd, const char * path, file finfo){
    char * fname = (char *)malloc(sizeof(char) * (strlen(path)+strlen(finfo.name) + 2));
    sprintf(fname, "%s/%s", path, finfo.name);
    FILE * fp = fopen(fname, "rb");
    if(fp == 0x0){
        printf("cannot open %s\n", path);
        free(fname);
        return -1;
    }
    int read_len;
    int buf[BUF_SIZE];
    while(1){
        read_len = fread(buf, 1, BUF_SIZE, fp);
        write(fd, buf, read_len);
        if(read_len < BUF_SIZE){
            break;
        }
    }
    free(fname);
    fclose(fp);
    return 0;
}

void print_files(file * files, int fcount){
    printf("name   size   mode\n");
    for(int i = 0; i < fcount; i++){
        printf("%s  %lu  %d\n", files[i].name, files[i].size, files[i].auth);
    }
}
