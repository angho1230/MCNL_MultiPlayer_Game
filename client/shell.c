#include <asm-generic/errno-base.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include "shell.h"



char * get_new_path(const char * path, const char * arg){
    if(strcmp(arg, ".") == 0){
        char * new_path = strdup(path);
        return new_path;
    }
    if(strcmp(arg, "..") == 0){
        char * new_path = strdup(path);
        char *curr, *prev;
        curr = new_path;
        prev = new_path;
        while(strchr(curr, '/') == 0x0){
            prev = curr;
            curr++;
        }
        if(prev != new_path){
            *prev = '\0';
        } else {
            *(prev + 1) = '\0';
        }
        return new_path;
    }
    char * new_path = (char *)malloc(sizeof(char) * (strlen(path) + strlen(arg)+2));
    sprintf(new_path, "%s/%s", path, arg);
    return new_path;
}


int change_dir(char ** path, const char * arg){
    char * new_path = get_new_path(*path, arg);
    if(access(new_path, X_OK) == -1){
        int err = errno;
        free(new_path);
        switch(err){
            case EACCES:
                printf("NO PERM\n");
                return NO_PERMISSION;
            case ENOENT:
                printf("NO FILE\n");
                return NO_FILE;
            case ENOTDIR:
                printf("NO DIR\n");
                return NO_DIR;
            default:
                printf("ERR\n");
                return UNDEFINED_ERR;
        }
    }
    free(*path);
    *path = new_path;
    return 0;
}

comm_st parse_command(char * comm_c){
    comm_st comm;
    if(strcmp(comm_c, "ls") == 0){
        comm.c = LS;
        return comm;
    }
    return comm;
}


