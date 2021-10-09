#include "utils.h"
#include "cmd_handle.h"
#include "socket_utils.h"
#include "path_utils.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int max(int a, int b) {
    if (a >= b) return a;
    return b;
}

int check_ipaddr(int ip) { //检验ip地址是否合法
    if(ip < 0 || ip > 255) return 0;
    return 1;
}

int check_port_invalid(int port) {
    int clnt_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &addr.sin_addr);
    if (bind(clnt_sock, (struct sockaddr *)(&addr), sizeof(addr)) == -1) return 1;
    close(clnt_sock);
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
        case 150:
            sprintf(resp_final, "%d %s\r\n", code, "RETR command success.");
            break;
        case 200:
            sprintf(resp_final, "%d %s\r\n", code, resp_msg);
            break;
        case 215:
            sprintf(resp_final, "%d %s\r\n", code, "UNIX Type: L8");
            break;
        case 220:
            sprintf(resp_final, "%d %s\r\n", code, "Hello.");
            break; 
        case 221:
            sprintf(resp_final, "%d %s\r\n", code, resp_msg);
            break;
        case 226:
            sprintf(resp_final, "%d %s\r\n", code, "FTP service completed.");
            break;
        case 227:
            sprintf(resp_final, "%d %s\r\n", code, resp_msg);
            break;
        case 230:
            sprintf(resp_final, "%d %s\r\n", code, "Guest login ok, access restrictions apply.");
            break;
        case 250:
            sprintf(resp_final, "%d %s\r\n", code, resp_msg);
            break;
        case 257:
            sprintf(resp_final, "%d %s\r\n", code, resp_msg);
            break;
        case 331:
            sprintf(resp_final, "%d %s\r\n", code, "Guest login ok, send your complete e-mail address as password.");
            break;
        case 425:
            sprintf(resp_final, "%d %s\r\n", code, "No TCP connection was established.");
            break;
        case 426:
            sprintf(resp_final, "%d %s\r\n", code, "FTP service is continuing.");
            break;
        case 500:
            sprintf(resp_final, "%d %s\r\n", code, "No command.");
            break;
        case 501:
            sprintf(resp_final, "%d %s\r\n", code, "Parameters number error.");
            break;
        case 502:
            sprintf(resp_final, "%d %s\r\n", code, "Invalid parameters.");
            break;
        case 504:
            sprintf(resp_final, "%d %s\r\n", code, "Parameters error.");
            break;
        case 530:
            sprintf(resp_final, "%d %s\r\n", code, "Permission denied.");
            break;
        case 550:
        sprintf(resp_final, "%d %s\r\n", code, "File does not exist or permission denied.");
        break;

    }
    send(clnt_sock, resp_final, strlen(resp_final), MSG_WAITALL);
    return;
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

int transfer(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    int mode = clients[idx].mode;
    //printf("MODE: %d\n", mode);
    if (mode == NO_CONNECTION) {
        send_response(clnt_sock, 425, NULL);
        return 0;
    } else if (mode == READY) {
        clients[idx].mode = PORT_MODE;
        int tr_sock = clients[idx].transfer_serve_sock;
        struct sockaddr_in addr = clients[idx].addr;
        if (connect(tr_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
            close(tr_sock);
            send_response(clnt_sock, 425, NULL);
            return 0;
        }
        send_response(clnt_sock, 150, NULL);
    } else if (mode == LISTENING) {
        clients[idx].mode = PASV_MODE;
        send_response(clnt_sock, 150, NULL);
    }
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    else {
        send_response(clnt_sock, 150, NULL);
    }
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    return 1;
}