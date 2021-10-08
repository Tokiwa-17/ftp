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

extern int listen_port;

int main(){
    listen_port = 21;
    int serve_sock = generate_sock(listen_port);
    if(serve_sock == -1) return 0;
    sock_init(serve_sock);
    int ready_idx = -1;
    fd_set fd_read_set, fd_write_set;
    while(1) {
        fd_read_set = handle_set;
        fd_write_set = handle_set;
        ready_idx = select(max_serve_sock, &fd_read_set, &fd_write_set, NULL, NULL);
        if (ready_idx == -1) {
            break;
        }
        if (FD_ISSET(serve_sock, &fd_read_set)) {
            
        }
    }
    return 0;
}
