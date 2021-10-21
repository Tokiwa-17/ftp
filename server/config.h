#pragma once
#define MAX_CLIENTS 10
#define BUFSIZE 8000

#define NOT_LOG_IN 0
#define LOGGING 1 
#define LOG_IN 2
#define TRANSFER 3
#define RNFR_CMP 4

#define NO_CONNECTION 0
#define PASV_MODE 1
#define READY 2
#define LISTENING 3
#define PORT_MODE 4
#define READY_TO_CONNECT 5

#define READ 1
#define WRITE 2
#define LIST 3

#include <sys/socket.h>
#include <netinet/in.h>
int listen_port;
int max_serve_sock;
int max_idx;        //正在连接的客户端最大下标
char LOCAL_IP[20];// = "192.168.44.133";
char PREFIX[100]; //= "/home/ylf/desktop/myFTP/ftp";
char ROOT[200];
fd_set handle_set; // 实际是一long类型的数组，每一个数组元素都能与一打开的文件句柄建立联系
//FD_SET(int fd, fd_set *fdset);       //将fd加入set集合
//FD_CLR(int fd, fd_set *fdset);       //将fd从set集合中清除
//FD_ISSET(int fd, fd_set *fdset);     //检测fd是否在set集合中，不在则返回0
//FD_ZERO(fd_set *fdset);              //将set清零使集合中不含任何fd

struct client_status
{
    int connect_serve_sock;
    int transfer_serve_sock;
    int state;                  // log in /not log in/file transfering, etc.)
    int mode;                   // file transfer mode, including preparing stages
    struct sockaddr_in addr;
    int rw_state;                     // read or write or list
    char filename[512];         // file to retrieve or store
    char url_prefix[100]; 
    char rename_file[512];      // file new name
    int offset;              // transfer offset
    int bytes_num;              // how many bytes
    int transfers_num;          // how many tranfers
};
struct client_status clients[MAX_CLIENTS];