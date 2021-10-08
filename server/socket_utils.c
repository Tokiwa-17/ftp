#include "utils.h"
#include "cmd_handle.h"
#include "socket_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int generate_sock(int port) {
    int serve_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serve_sock == 1) return -1;
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(INADDR_ANY);
    serv_addr.sin_port = htons(port);
    if (bind(serve_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1) return -1;
    if (listen(serve_sock, 10) == -1) return -1;
    return serve_sock;
}

void sock_init(int sock) {
    
}