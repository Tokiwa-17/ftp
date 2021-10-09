#include "utils.h"
#include "cmd_handle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int check_ipaddr(int ip) { //检验ip地址是否合法
    if(ip < 0 || ip > 255) return 0;
    return 1;
}

int check_port_invalid(int port) {
    return 0;
}

void send_test(int serve_sock, char * buf) {
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_addr.s_addr = inet_addr("192.168.44.133");
    serv_addr.sin_port = htons(1234); 
    bind(serve_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serve_sock, 10);
    printf("Wait for Connection..................\n");
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serve_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    send(clnt_sock, buf, sizeof(buf) - 1, 0);
}


void send_response(int clnt_sock, int code, char *resp_msg) {
    char resp_final[200];
    switch (code) {
        case 220:
            sprintf(resp_final, "%d %s\r\n", code, "Hello.");
            break; 
        case 230:
            sprintf(resp_final, "%d %s\r\n", code, "Guest login ok, access restrictions apply.");
            break;
        case 500:
            sprintf(resp_final, "%d %s\r\n", code, "No command.");
            break;
        case 530:
            sprintf(resp_final, "%d %s\r\n", code, "Permission denied.");
            break;
        case 331:
            sprintf(resp_final, "%d %s\r\n", code, "Guest login ok, send your complete e-mail address as password.");
            break;
    }
    send(clnt_sock, resp_final, strlen(resp_final), MSG_WAITALL);
    return;
}

void get_absolute_path(char *prefix, char *src, char *dest)
{
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
int clear_crlf(char *str, int len) {
    int i = len - 1;
    while (str[i] == '\r' || str[i] == '\n') {
        str[i] = '\0';
        i--;
    }
    return i + 1;
}

int recv_from_client(int clnt_sock, int idx) {
    char recv_msg[1000];
    char command[30];
    char others[30];
    char param[30];
    int length = 0;
    length = recv(clnt_sock, recv_msg, 1000, 0);
    if(length <= 0) return 0;
    //printf("LENGTH: %d\n", length);
    recv_msg[length] = '\0';
    //分解命令
    int param_num = sscanf(recv_msg, "%s %s", command, others);
    clear_crlf(command, strlen(command));
    if(param_num <= 0) {
        // code == 500
        send_response(clnt_sock, 500, NULL);
    } else if (param_num == 1) {
        cmd_handler(command, NULL, idx);
    } else {
        strcpy(param, recv_msg + strlen(command) + 1);
        clear_crlf(param, strlen(param));
        cmd_handler(command, param, idx);
    }
    //printf("TEST_RECV_MSG: %s\n", recv_msg);
    return 1;
}