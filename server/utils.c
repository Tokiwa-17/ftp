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

    }
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
int strip_crlf(char *sentence, int len)
{
    int i = len - 1;
    while (sentence[i] == '\r' || sentence[i] == '\n')
    {
        sentence[i] = '\0';
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
    printf("LENGTH: %d\n", length);
    recv_msg[length] = '\0';
    //分解命令
    int param_num = sscanf(recv_msg, "%s %s", command, others);
    if(param_num <= 0) {
        // code == 500
    } else if (param_num == 1) {
        //send_response(command, NULL, idx);
    } else {
        strcpy(param, recv_msg + strlen(command) + 1);
        //send_response(command, param, idx);
    }
    //length = strip_crlf(recv_msg, length);
    printf("TEST_RECV_MSG: %s\n", recv_msg);
    return 1;
}