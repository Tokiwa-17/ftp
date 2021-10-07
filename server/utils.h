# pragma once
#include <sys/socket.h>
#include <netinet/in.h>
int check_ipaddr(int);

struct client_status
{
    int connect_fd;  // fd for control 
    int transfer_fd; // fd for file transfer
    int state;       // state(logged in/not logged in/file transfering, etc.)
    int mode; // file transfer mode, including preparing stages
    struct sockaddr_in addr;
    int rw; // read or write or list
    char filename[512]; // file to retrieve or store
    char prefix[128]; 
    char rename_file[512]; // file new name
    int start_pos; // transfer offset
    int bytes; // how many bytes
    int transfers; // how many tranfers
};

void send_test(int serve_sock, char *buf);