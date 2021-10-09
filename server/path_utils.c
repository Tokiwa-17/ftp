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

void get_absolute_path(char *prefix, char *src, char *dest){
    int len = strlen(prefix);
    if (src[0] == '/') // 给定的路径是绝对路径
        sprintf(dest, "%s", src);
    else if (src[0] == '.') { // 给定的路径是相对路径
        if (prefix[len - 1] == '/') 
            sprintf(dest, "%s%s", prefix, src + 2);
        else
            sprintf(dest, "%s/%s", prefix, src + 2);
    }
    else {
        if (prefix[len - 1] == '/')
            sprintf(dest, "%s%s", prefix, src);
        else
            sprintf(dest, "%s/%s", prefix, src);
    }
}

/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
access: https://baike.baidu.com/item/access/5245577 
常见宏:https://blog.csdn.net/yasi_xi/article/details/9226267
stat结构体:https://www.cnblogs.com/yaowen/p/4801541.html
>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
int check_folder(char *path) {
    struct stat file;
    if (access(path, F_OK) != 0 || stat(path, &file) != 0 || !S_ISDIR(file.st_mode))
        return 0; 
    return 1;
}

int del_dir(char *path) {
    //成功返回1
    struct stat dir;
    if (access(path, F_OK) != 0) return 0; // 文件不存在
    if (stat(path, &dir) == -1 ) return 0; // 获取文件信息失败
    // TODO 是否是一个常规文件
    if (S_ISDIR(dir.st_mode) && !rmdir(path)) return 1;
    return 0;
}