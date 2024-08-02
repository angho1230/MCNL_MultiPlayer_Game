typedef enum {
    CD,
    LS,
    UP,
    DL,
    EXIT
}comm_e;

typedef enum{
    SUCCESS,
    NO_PERMISSION,
    NO_DIR,
    IS_DIR,
    NO_FILE,
    UNDEFINED_ERR,
}res_e;

typedef struct {
    comm_e c;
    char arg[1024];
}comm_st;

typedef struct{
    res_e code;
    int size;//size of the data that will be transferred
}res_st;
char * get_new_path(const char * path, const char * arg);
int change_dir(char ** path, const char * arg);
comm_st parse_command(char * comm);
