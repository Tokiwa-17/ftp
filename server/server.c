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
        ready_idx = select(max_serve_sock + 1, &fd_read_set, &fd_write_set, NULL, NULL);
        if (ready_idx == -1) {
            //printf("ready_idx == -1\n");
            break;
        }
        if (FD_ISSET(serve_sock, &fd_read_set)) {
            //printf("TTTTTTTTTTTTTTTTTTTTTTT\n");
            int clnt_sock = accept(serve_sock, NULL, NULL);
            if (clnt_sock == -1) {
                return 0;
            } else {
                if (!manage_fds(clnt_sock))
                    close_fd(clnt_sock);
            }
        }
        for(int i = 0; i <= max_idx; i++) {
            int clnt_sock_tmp = clients[i].connect_serve_sock;
            //printf("clnt_sock_tmp: %d\n", clnt_sock_tmp);
            if (clnt_sock_tmp == -1) continue;
            if (FD_ISSET(clnt_sock_tmp, &fd_read_set)) {
               if (recv_from_client(clnt_sock_tmp, i) == 0) {
                   // TODO:关闭连接
                   // 关闭控制
                   return 0;
               }
            }


        }
    }


    close(serve_sock);
    return 0;
}
