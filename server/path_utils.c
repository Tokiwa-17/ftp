#include "path_utils.h"
#include "utils.h"
#include "cmd_handle.h"
#include "socket_utils.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/types.h>    
#include <sys/stat.h> 
#include <unistd.h>
int check_folder(char *path) {
    struct stat file;
    if (access(path, F_OK) != 0 || stat(path, &file) != 0 || !S_ISDIR(file.st_mode))
        return 0; // https://baike.baidu.com/item/access/5245577
    return 1;
}