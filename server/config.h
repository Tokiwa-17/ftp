# pragma once
# define MAX_CLIENTS 10
#include <sys/socket.h>
#include <netinet/in.h>
int listen_port;
char LOCAL_IP[20] = "192.168.44.133";
char PREFIX[100] = "/home/ylf/desktop/myFTP/ftp";


struct client_status
{
    int connect_fd;  // fd for control 文件描述符
    int transfer_fd; // fd for file transfer
    int state;       // state(logged in/not logged in/file transfering, etc.)
    int mode; // file transfer mode, including preparing stages
    struct sockaddr_in addr;
    int rw; // read or write or list
    char filename[512]; // file to retrieve or store
    char url_prefix[100]; 
    char rename_file[512]; // file new name
    int start_pos; // transfer offset
    int bytes; // how many bytes
    int transfers; // how many tranfers
};
struct client_status clients[MAX_CLIENTS];