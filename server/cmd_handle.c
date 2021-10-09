#include "utils.h"
#include "cmd_handle.h"
#include "config.h"
#include "socket_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern struct client_status clients[MAX_CLIENTS];

void USER(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    int state = clients[idx].state;
    printf("%s\n", param);
    printf("anonymous\n");
    printf("TEST: %d\n", strcmp(param, "anonymous"));
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
    } else if (state != NOT_LOG_IN) {
        send_response(clnt_sock, 530, NULL);
    } else if (strcmp(param, "anonymous") == 0) {
        clients[idx].state = LOG_IN;
        send_response(clnt_sock, 331, NULL);
    } else send_response(clnt_sock, 530, NULL);
}

void PORT(char *param, int idx) {
    // TODO:如果 param == NULL
    int h1, h2, h3, h4, p1, p2;
    int cnt = sscanf(param, "%d %d %d %d %d %d", &h1, &h2, &h3, &h4, &p1, &p2);
    if(cnt != 6) {
        printf("%d, params error!\n", cnt);
        return;
    }
    if( !check_ipaddr(h1) || !check_ipaddr(h2) || !check_ipaddr(h3) || !check_ipaddr(h4)) {
        printf("IP address error!\n");
        return;
    }
    // TODO:如果当前客户端已经处于连接状态， 关闭该连接
    char ip[20];
    sprintf(ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    //printf("IPAddr: %s\n",ip);
    // TODO:更改客户端的状态为准备连接
    int port = p1 * 256 + p2;

    if(inet_pton(AF_INET, ip, &(clients[idx].addr.sin_addr)) != 1) {
        //将点分十进制的ip地址转化为用于网络传输的数值格式
        //返回值：若成功则为1，若输入不是有效的表达式则为0，若出错则为-1
        printf("IP address conversion failed!\n");
        return ;
    }
    int serv_sock;
    if((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Socket establish failed!\n");
        return ;
    }
    // TODO manage_trans_fds
    char buf[1024];
    strcpy(buf, "PORT CMD TEST\r\n");
    printf("buf_test: %s\n", buf);
    send_test(serv_sock, buf);
}

void PASV(char *param, int idx) {
    
    if (param != NULL) {
        printf("Params error!\n");
        return ; 
    }
    // TODO 如果已经存在一个连接
    int port = rand() % 45536 + 20000;
    while(check_port_invalid(port)) {
        port = rand() % 45536 + 20000;
    }
    int h1, h2, h3, h4, p1, p2;
    sscanf(LOCAL_IP, "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
    p1 = port / 256;
    p2 = port % 256;
    char resp_msg[50];
    sprintf(resp_msg, "Entering Passive Mode (%d,%d,%d,%d,%d,%d)", h1, h2, h3, h4, p1, p2);
    //printf("test_resp_msg: %s\n", resp_msg);
    int serv_sock;
    if((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Socket establish failed!\n");
        return ;
    }
    // TODO manage_trans_fds
    send_test(serv_sock, resp_msg);
}

void RETR(char *param, int idx) {
    int serve_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (param == NULL) {
        send_response(serve_sock, 504, NULL);
        return ;
    }
    char absolute_path[100];
    get_absolute_path(PREFIX, param, absolute_path);
    printf("Absolute_path test: %s\r\n", absolute_path);
    FILE *f;
    if ((f = fopen(absolute_path, "rb+")) == NULL){
        printf("File Open Failed!\n");
        send_response(serve_sock, 550, NULL);
        return;
    }
    else {
        fclose(f);
        printf("File Open Success!\n");
    }
    // TODO 更新客户端的状态

}

void STOR(char *param, int idx) {
    int serve_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP); 
    if (param == NULL) {
        send_response(serve_sock, 504, NULL);
        return ;
    }
    char absolute_path[100];
    get_absolute_path(PREFIX, param, absolute_path);
    printf("Absolute_path test: %s\r\n", absolute_path);
    FILE *f;
    if ((f = fopen(absolute_path, "rb+")) == NULL){
        printf("File Open Failed!\n");
        send_response(serve_sock, 550, NULL);
        return;
    }
    else {
        fclose(f);
        printf("File Open Success!\n");
    }
    // TODO 更新客户端的状态

}

void SYST(char *param, int idx) {

    if (param != NULL) {
        printf("Param error!\n");
        return;
    }
    char resp_msg[30] = "215 UNIX Type: L8\r\n";
    int serv_sock;
    if((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Socket establish failed!\n");
        return ;
    }
    // TODO manage_trans_fds
    send_test(serv_sock, resp_msg);
}

void TYPE(char *param, int idx) {

    if (param == NULL) {
        printf("Param error!\n");
        return ;
    }
    if (strcmp(param, "I") == 0) {
        char resp_msg[30] = "200 Type set to I.\r\n";
        int serv_sock;
        if((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            printf("Socket establish failed!\n");
            return ;
        }
        // TODO manage_trans_fds
        send_test(serv_sock, resp_msg);
    }
}

void QUIT(char *param, int idx) {
    if(param != NULL) {
        printf("Param error!\n");
        return;
    }
    char file_info[50], byte_info[50];
    // TODO 记录传输的文件数和字节数
    int files_num = 0, bytes_num = 0;
    sprintf(file_info, "Total number of files transferred: %d", files_num);
    sprintf(byte_info, "Total number of bytes transferred: %d", bytes_num);
    char resp_msg[100];
    sprintf(resp_msg, "200\r\n %s\r\n %s\r\n", file_info, byte_info);
    //printf("test_quit: %s", resp_msg);
    int serv_sock;
    if((serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        printf("Socket establish failed!\n");
        return ;
    }
    // TODO manage_trans_fds
    send_test(serv_sock, resp_msg);
}

void ABOR(char *param, int idx) {
    int serve_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(param != NULL) {
        send_response(serve_sock, 504, NULL);
        return ;
    }
    send_response(serve_sock, 226, NULL);
}

void cmd_handler(char *cmd, char *param, int idx) {
    if (strcmp(cmd, "USER") == 0) {
        USER(param, idx);
    }

    if (strcmp(cmd, "PORT") == 0) {
        PORT(param, idx);
    }
    if (strcmp(cmd, "PASV") == 0) {
        PASV(param, idx);
    }
    if (strcmp(cmd, "TYPE") == 0) {
        TYPE(param, idx);
    }
    if (strcmp(cmd, "QUIT") == 0) {
        QUIT(param, idx);
    }
}