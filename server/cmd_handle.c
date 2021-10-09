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
    //printf("TEST: %d\n", strcmp(param, "anonymous"));
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
    } else if (state != NOT_LOG_IN) {
        send_response(clnt_sock, 530, NULL);
    } else if (strcmp(param, "anonymous") == 0) {
        clients[idx].state = LOGGING;
        send_response(clnt_sock, 331, NULL);
    } else send_response(clnt_sock, 530, NULL);
}

void PASS(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    int state = clients[idx].state;
    if (state != LOGGING) {
        // USER -> LOGGING
        send_response(clnt_sock, 530, NULL);
        return;
    }
    clients[idx].state = LOG_IN;
    send_response(clnt_sock, 230, NULL);
}

void PORT(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    int state = clients[idx].state;
    int tr_sock = clients[idx].transfer_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    int h1, h2, h3, h4, p1, p2;
    int cnt = sscanf(param, "%d %d %d %d %d %d", &h1, &h2, &h3, &h4, &p1, &p2);
    if (cnt != 6) {
        send_response(clnt_sock, 501, NULL);
        return;
    }
    if ( !check_ipaddr(h1) || !check_ipaddr(h2) || !check_ipaddr(h3) || !check_ipaddr(h4)) {
        send_response(clnt_sock, 502, NULL);
        return;
    }
    // 如果当前客户端已经处于连接状态， 关闭该连接
    if (tr_sock != -1) {
        close(clnt_sock);
        clients[idx].transfer_serve_sock = -1;
        clients[idx].state = LOG_IN;
        clients[idx].mode = NO_CONNECTION;
        clients[idx].offset = 0;
        FD_CLR(clnt_sock, &handle_set);
    }
    char ip[20];
    sprintf(ip, "%d.%d.%d.%d", h1, h2, h3, h4);
    // 更改客户端的状态为准备连接
    clients[idx].mode = READY;
    int port = p1 * 256 + p2;
    memset(&(clients[idx].addr), 0, sizeof(clients[idx].addr));
    clients[idx].addr.sin_family = AF_INET;
    clients[idx].addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &(clients[idx].addr.sin_addr)) != 1) {
        send_response(clnt_sock, 530, NULL);
        return ;
    }
    if((tr_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        send_response(clnt_sock, 500, NULL);
        return ;
    }
    update_trans_sock(tr_sock, idx);
    //clients[idx].transfer_serve_sock = tr_sock;
    //FD_SET(tr_sock, &handle_set);
    //max_serve_sock = max(tr_sock, max_serve_sock);
    send_response(clnt_sock, 200, NULL);
}

void PASV(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    int tr_sock = clients[idx].transfer_serve_sock;
    if (param != NULL) {
        send_response(clnt_sock, 504, NULL);
        return ; 
    }
    if (tr_sock != -1) {
        int tmp_tr_sock = clients[idx].transfer_serve_sock;
        if (tmp_tr_sock != -1) {
            close(tmp_tr_sock);
            clients[idx].transfer_serve_sock = -1;
            clients[idx].state = LOG_IN;
            clients[idx].mode = NO_CONNECTION;
            clients[idx].offset = 0;
            FD_CLR(tmp_tr_sock, &handle_set);
        }
    }
    int port = rand() % 45536 + 20000;
    while(check_port_invalid(port)) {
        port = rand() % 45536 + 20000;
    }
    printf("TEST_PORT: %d\n", port);
    int h1, h2, h3, h4, p1, p2;
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    strcpy(LOCAL_IP, "0.0.0.0");
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    sscanf(LOCAL_IP, "%d.%d.%d.%d", &h1, &h2, &h3, &h4);
    p1 = port / 256;
    p2 = port % 256;
    char resp_msg[100];
    sprintf(resp_msg, "Entering Passive Mode (%d,%d,%d,%d,%d,%d)", h1, h2, h3, h4, p1, p2);
    //printf("%s\n", resp_msg);
    clients[idx].mode = LISTENING;
    tr_sock = generate_sock(port);
    if(tr_sock == -1) {
        send_response(clnt_sock, 500, NULL);
        return;
    }
    update_trans_sock(tr_sock, idx);
    //clients[idx].transfer_serve_sock = tr_sock;
    //FD_SET(tr_sock, &handle_set);
    //max_serve_sock = max(tr_sock, max_serve_sock);
    send_response(clnt_sock, 227, resp_msg);
}

void RETR(char *param, int idx) {
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    strcpy(ROOT, "/home/ylf/desktop/myFTP/ftp");
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return ;
    }
    char absolute_path[100];
    get_absolute_path(clients[idx].url_prefix, param, absolute_path);
    int len = strlen(ROOT);
    if (ROOT[len - 1] == '/')   sprintf(clients[idx].filename, "%s%s", ROOT, absolute_path + 1);
    else sprintf(clients[idx].filename, "%s/%s", ROOT, absolute_path + 1);
    FILE *f;
    printf("PATH: %s\n", clients[idx].filename);
    if ((f = fopen(clients[idx].filename, "rb+")) == NULL){
        printf("File Open Failed!\n");
        send_response(clnt_sock, 530, NULL);
        return;
    }
    else fclose(f);
    // TODO 更新客户端的状态
    if (!transfer(param, idx)) return;
    clients[idx].transfers_num += 1;
    clients[idx].rw_state = READ;
    clients[idx].state =TRANSFER;
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
    if (strcmp(cmd, "USER") == 0) USER(param, idx);
    if (strcmp(cmd, "PASS") == 0) PASS(param, idx);
    if (strcmp(cmd, "PORT") == 0) PORT(param, idx);
    if (strcmp(cmd, "PASV") == 0) PASV(param, idx);
    if (strcmp(cmd, "RETR") == 0) RETR(param, idx);
    if (strcmp(cmd, "TYPE") == 0) TYPE(param, idx);
    if (strcmp(cmd, "QUIT") == 0) QUIT(param, idx);
}