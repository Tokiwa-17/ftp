#include "utils.h"
#include "cmd_handle.h"
#include "config.h"
#include "socket_utils.h"
#include "path_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>

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
    if (param == NULL) 
        send_response(clnt_sock, 504, NULL);
    clients[idx].state = LOG_IN;
    send_response(clnt_sock, 230, NULL);
}

void PORT(char *param, int idx) {
    printf("Param :%s\n", param);
    int clnt_sock = clients[idx].connect_serve_sock;
    int state = clients[idx].state;
    int tr_sock = clients[idx].transfer_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    int h1, h2, h3, h4, p1, p2;
    int cnt = sscanf(param, "%d,%d,%d,%d,%d,%d", &h1, &h2, &h3, &h4, &p1, &p2);
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
    clients[idx].mode = READY_TO_CONNECT;
    int port = p1 * 256 + p2;
    memset(&(clients[idx].addr), 0, sizeof(clients[idx].addr));
    clients[idx].addr.sin_family = AF_INET;
    clients[idx].addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip, &(clients[idx].addr.sin_addr)) <= 0) {
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
    send_response(clnt_sock, 200, "PORT success.");
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
    //printf("TEST_PORT: %d\n", port);
    int h1, h2, h3, h4, p1, p2;
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //strcpy(LOCAL_IP, "0.0.0.0");
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
    //strcpy(ROOT, "/home/ylf/desktop/myFTP/ftp");
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return ;
    }
    /*char absolute_path[100];
    get_absolute_path(clients[idx].url_prefix, param, absolute_path);
    // absolute_path = clients[idx].url_prefix + param
    int len = strlen(ROOT);
    if (ROOT[len - 1] == '/')   {
        if (absolute_path[0] == '/')
            sprintf(clients[idx].filename, "%s%s", ROOT, absolute_path + 1);
        else sprintf(clients[idx].filename, "%s%s", ROOT, absolute_path);
    }
    else {
        if (absolute_path[0] != '/')
            sprintf(clients[idx].filename, "%s/%s", ROOT, absolute_path);
        else sprintf(clients[idx].filename, "%s%s", ROOT, absolute_path);
    }*/
    strcpy(clients[idx].filename, param);
    printf("%s\n", clients[idx].filename);
    FILE *f;
    //printf("PATH: %s\n", clients[idx].filename);
    if ((f = fopen(clients[idx].filename, "rb+")) == NULL){
        printf("File Open Failed!\n");
        send_response(clnt_sock, 550, NULL);
        return;
    }
    else fclose(f);
    // TODO 更新客户端的状态
    if (!transfer(param, idx)) return;
    clients[idx].transfers_num += 1;
    clients[idx].rw_state = READ;
    clients[idx].state = TRANSFER;
}

void STOR(char *param, int idx) {
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //strcpy(ROOT, "/home/ylf/desktop/myFTP/ftp");
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return ;
    }
    /*char absolute_path[100];
    get_absolute_path(clients[idx].url_prefix, param, absolute_path);
    int len = strlen(ROOT);
    if (ROOT[len - 1] == '/')   sprintf(clients[idx].filename, "%s%s", ROOT, absolute_path + 1);
    else sprintf(clients[idx].filename, "%s%s", ROOT, absolute_path);
    */
    strcpy(clients[idx].filename, param);
    FILE *f;
    if ((f = fopen(clients[idx].filename, "ab+")) == NULL) { // appending + binary + reading
        printf("File Open Failed!\n");
        send_response(clnt_sock, 530, NULL);
        return;
    }
    else fclose(f);
    if (!transfer(param, idx)) return;
    clients[idx].transfers_num += 1;
    clients[idx].rw_state = WRITE;
    clients[idx].state = TRANSFER;
}

void SYST(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param != NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    send_response(clnt_sock, 215, NULL);
}

void TYPE(char *param, int idx) {

    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return ;
    } else if (strcmp(param, "I") == 0) {
        char resp_msg[30] = "Type set to I.";
        send_response(clnt_sock, 200, resp_msg);
    } 
    else send_response(clnt_sock, 502, NULL);
}

void QUIT(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param != NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    char logout_info[100];
    sprintf(logout_info, "You have transferred %d bytes in %d files.", clients[idx].bytes_num, clients[idx].transfers_num);
    int tr_sock = clients[idx].transfer_serve_sock;
    send_response(clnt_sock, 221, logout_info);
    close_transfer_fd(idx);
    close_fd(idx);
}

void ABOR(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param != NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }   
    if (clients[idx].transfer_serve_sock != -1) {
        int tr_sock = clients[idx].transfer_serve_sock;
        close(tr_sock);
        clients[idx].transfer_serve_sock = -1;
        clients[idx].state = LOG_IN;
        clients[idx].mode = NO_CONNECTION;
        clients[idx].offset = 0;
        FD_CLR(tr_sock, &handle_set);
        //send_response(clnt_sock, 426, NULL);
    } 
    send_response(clnt_sock, 226, NULL);
}

void MKD(char *param, int idx) {
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //strcpy(ROOT, "/home/ylf/desktop/myFTP/ftp");
    //>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    //printf("param: %s\n", param);
    char path[200], absolute_path[200];
    get_absolute_path(clients[idx].url_prefix, param, path);
    int len = strlen(ROOT);
    //if(ROOT[len - 1] == '/')   sprintf(absolute_path, "%s%s", ROOT, path + 1);
    //else sprintf(absolute_path, "%s%s", ROOT, path);
    //printf("%s\n", absolute_path);
    //printf("%s\n", path);
    if (mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0) 
        send_response(clnt_sock, 250, path);
    else send_response(clnt_sock, 550, NULL);
}

void CWD(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    char path[200], absolute_path[200];
    get_absolute_path(clients[idx].url_prefix, param, path);
    // url_prefix/param -> path
    printf("%s\n", path);
    int len = strlen(ROOT);
    if(ROOT[len - 1] == '/')   {
        if(path[0]=='/')
            sprintf(absolute_path, "%s%s", ROOT, path + 1);
        else sprintf(absolute_path, "%s%s", ROOT, path);
    }
    else {
        if(path[0]=='/')
            sprintf(absolute_path, "%s%s", ROOT, path);
        else sprintf(absolute_path, "%s/%s", ROOT, path);
    }
    // ROOT/path -> absolute_path
    printf("absolute_path: %s\n", absolute_path);
    if (check_folder(absolute_path)) {
        strcpy(clients[idx].url_prefix, path);
        // clients[idx].url_prefix记录没有ROOT的绝对路径
        send_response(clnt_sock, 250, NULL);
    }
    else send_response(clnt_sock, 550, NULL);

}

void PWD(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param != NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    char resp_msg[200];
    sprintf(resp_msg, "\"%s\"", clients[idx].url_prefix);
    printf("pwd: %s\n", resp_msg);
    send_response(clnt_sock, 257, resp_msg);
}

void LIST_(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param != NULL) {
        char path[200];
        get_absolute_path(clients[idx].url_prefix, param, path);
        int len = strlen(ROOT);
        //if(ROOT[len - 1] == '/')   sprintf(path, "%s%s", ROOT, clients[idx].filename);
        //else sprintf(path, "%s%s", ROOT, clients[idx].filename);
        if(ROOT[len - 1] == '/') {
            if (path[0] == '/') sprintf(clients[idx].filename, "%s%s", ROOT, path + 1);
            else sprintf(clients[idx].filename, "%s%s", ROOT, path);
        } else {
            if (path[0] == '/') sprintf(clients[idx].filename, "%s%s", ROOT, path);
            else sprintf(clients[idx].filename, "%s/%s", ROOT, path);
        }
        //printf("ROOT: %s\n", ROOT);
        //printf("param: %s\n", param);
        //printf("filename: %s\n", clients[idx].filename);
        if (!check_file(clients[idx].filename) && !check_folder(clients[idx].filename)) {
            //send_response(clnt_sock, 451, NULL);
            //return;
            if(!check_file(param) && !check_folder(param)) {
                send_response(clnt_sock, 451, NULL);
                return;
            } else {
                strcpy(clients[idx].filename, param);
                if (!transfer(clients[idx].filename, idx)) return;
                clients[idx].rw_state = LIST;
            }
        } else {
            if(!transfer(clients[idx].filename, idx)) return;
            clients[idx].rw_state = LIST;
        }
    } else {
            strcpy(clients[idx].filename, "/tmp");
            if(!transfer(clients[idx].filename, idx)) return;
            clients[idx].rw_state = LIST;
    }
}

void RMD(char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    char path[200], absolute_path[200];
    //get_absolute_path(clients[idx].url_prefix, param, path);
    //int len = strlen(ROOT);
    //if(ROOT[len - 1] == '/')   sprintf(absolute_path, "%s%s", ROOT, path + 1);
    //else sprintf(absolute_path, "%s%s", ROOT, path);
    //printf("ABSPATH: %s\n", absolute_path);
    if (del_dir(param)) send_response(clnt_sock, 250, NULL);
    else send_response(clnt_sock, 550, NULL);
}

void RNFR(char *param, int idx) { // 重命名开始
    int clnt_sock = clients[idx].connect_serve_sock;
    if(param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    /*char path[200], absolute_path[200];
    get_absolute_path(clients[idx].url_prefix, param, path);
    int len = strlen(ROOT);
    if(ROOT[len - 1] == '/') {
        if(path[0] == '/')
            sprintf(absolute_path, "%s%s", ROOT, path + 1);
        else sprintf(absolute_path, "%s%s", ROOT, path);
    }
    else {
        if(path[0] == '/')
            sprintf(absolute_path, "%s%s", ROOT, path);
        else sprintf(absolute_path, "%s/%s", ROOT, path);
    }*/
    //printf("ABS_PATH: %s\n", absolute_path);
    if(check_file(param)) {
        clients[idx].state = RNFR_CMP;
        strcpy(clients[idx].rename_file, param);
        send_response(clnt_sock, 350, NULL);
    } 
    else send_response(clnt_sock, 550, NULL);
}

void RNTO(char *param, int idx) { // 重命名结束
    int clnt_sock = clients[idx].connect_serve_sock;
    if (clients[idx].state != RNFR_CMP) {
        send_response(clnt_sock, 503, "RNFR required.");
        return ;
    }
    clients[idx].state = LOG_IN;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    /*char path[200], absolute_path[200], cmd[400];
    get_absolute_path(clients[idx].url_prefix, param, path);
    int len = strlen(ROOT);
    if(ROOT[len - 1] == '/') {
        if(path[0] == '/')
            sprintf(absolute_path, "%s%s", ROOT, path + 1);
        else sprintf(absolute_path, "%s%s", ROOT, path);
    }
    else {
        if(path[0] == '/')
            sprintf(absolute_path, "%s%s", ROOT, path);
        else sprintf(absolute_path, "%s/%s", ROOT, path);
    }*/
    char cmd[400];
    sprintf(cmd, "mv %s %s", clients[idx].rename_file, param);
    // mv src dest
    if(system(cmd) == -1) {
        send_response(clnt_sock, 550, NULL);
        return;
    } else {
        char resp_msg[100];
        strcpy(resp_msg, "RNTO success.");
        send_response(clnt_sock, 250, resp_msg);
    }
}

void REST (char *param, int idx) {
    int clnt_sock = clients[idx].connect_serve_sock;
    if (param == NULL) {
        send_response(clnt_sock, 504, NULL);
        return;
    }
    int restart_pos = -1;
    if(sscanf(param, "%d", &restart_pos) == 1 && restart_pos) {
        clients[idx].offset = restart_pos;
        send_response(clnt_sock, 350, "Restart success.");
    }
    else send_response(clnt_sock, 501, NULL);
}

void cmd_handler(char *cmd, char *param, int idx) {
    if (strcmp(cmd, "USER") == 0) USER(param, idx);
    if (strcmp(cmd, "PASS") == 0) PASS(param, idx);
    if (strcmp(cmd, "PORT") == 0) PORT(param, idx);
    if (strcmp(cmd, "PASV") == 0) PASV(param, idx);
    if (strcmp(cmd, "RETR") == 0) RETR(param, idx);
    if (strcmp(cmd, "STOR") == 0) STOR(param, idx);
    if (strcmp(cmd, "SYST") == 0) SYST(param, idx);
    if (strcmp(cmd, "TYPE") == 0) TYPE(param, idx);
    if (strcmp(cmd, "QUIT") == 0) QUIT(param, idx);
    if (strcmp(cmd, "ABOR") == 0) ABOR(param, idx);
    if (strcmp(cmd, "MKD")  == 0) MKD(param, idx);
    if (strcmp(cmd, "CWD")  == 0) CWD(param, idx);
    if (strcmp(cmd, "PWD")  == 0) PWD(param, idx);
    if (strcmp(cmd, "LIST") == 0) LIST_(param, idx);
    if (strcmp(cmd, "RMD")  == 0) RMD(param, idx);
    if (strcmp(cmd, "RNFR") == 0) RNFR(param, idx);
    if (strcmp(cmd, "RNTO") == 0) RNTO(param, idx);
    if (strcmp(cmd, "REST") == 0) REST(param, idx);
}