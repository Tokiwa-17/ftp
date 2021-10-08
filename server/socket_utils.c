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

extern int max_serve_sock;
extern fd_set handle_set;
extern struct client_status clients[MAX_CLIENTS];

int generate_sock(int port) {
    int serve_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serve_sock == -1) return -1;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    if (bind(serve_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) return -1;
    if (listen(serve_sock, 10) == -1) return -1;
    return serve_sock;
}

void sock_init(int sock) {
    max_serve_sock = sock;
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].connect_serve_sock = -1;
        clients[i].transfer_serve_sock = -1;
        clients[i].state = NOT_LOG_IN;
        clients[i].mode = NO_CONNECTION;
        strcpy(clients[i].url_prefix, "/");
        clients[i].offset = 0;
        clients[i].bytes_num = 0;
        clients[i].transfers_num = 0;
    }
    FD_ZERO(&handle_set);
    FD_SET(sock, &handle_set);
}

int manage_fds(int cur_fd) {
    int i = 0;
    for (;i < MAX_CLIENTS;i++) {
        if (clients[i].connect_serve_sock == -1) {
            clients[i].connect_serve_sock = cur_fd;
            break;
        }
    }
    if (i == MAX_CLIENTS) return 0;
    FD_SET(cur_fd, &handle_set);
    if (cur_fd > max_serve_sock) max_serve_sock = cur_fd;
    if (i > max_idx) max_idx = i;
    return 1;
}

void close_fd(int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if(clnt_sock == -1) {
        close(clnt_sock);
        clients[idx].connect_serve_sock = -1;
        clients[idx].state = NOT_LOG_IN;
        clients[idx].mode = NO_CONNECTION;
        clients[idx].offset = 0;
        clients[idx].transfers_num = 0;
        clients[idx].bytes_num = 0;
        strcpy(clients[idx].url_prefix, "/");
        FD_CLR(clnt_sock, &handle_set);
    }
}