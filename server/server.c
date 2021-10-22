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
#include <time.h>
#include <getopt.h>
#include <errno.h>

extern int listen_port;

int main(int argc, char **argv){
    srand((unsigned)time(NULL));
    if(!get_local_IPaddr(&LOCAL_IP)) return 0;
    strcpy(ROOT, "/tmp");
    listen_port = 21;

    int opt;
    char check_c;
    const struct option arg_options[] = 
    {
        {"port", required_argument, NULL, 'p'},

        {"root", required_argument, NULL, 'r'},

        {NULL, 0, NULL, 0},
    };

    while ((opt = getopt_long_only(argc, (char *const *)argv, "p:r:", arg_options, NULL)) != -1)
    {
        switch (opt)
        {
        case 'r':
            if (access(optarg, 0) == -1)
            {
                return 0;
            }
            strcpy(ROOT, optarg);
            break;
        case 'p':
            if (sscanf(optarg, "%d%c", &listen_port, &check_c) != 1)
            {
                return 0;
            }
            if (listen_port < 1 || listen_port > 65535)
            {
                return 0;
            }
            break;
        case '?':
            return 0;
        }
    }

    int serve_sock = generate_sock(listen_port);
    if(serve_sock == -1) return 0;
    sock_init(serve_sock);
    int ready_idx;
    fd_set fd_read_set, fd_write_set;
    while(1) {
        fd_read_set = handle_set;
        fd_write_set = handle_set;
        ready_idx = select(max_serve_sock + 1, &fd_read_set, &fd_write_set, NULL, NULL);
        //https://blog.csdn.net/u014530704/article/details/72833186
        //select()调用返回处于就绪状态并且已经包含在fd_set结构中的描述字总数
        if (ready_idx == -1) {
            if (errno == EINTR) continue;
            else break;
        }
        if (FD_ISSET(serve_sock, &fd_read_set)) {
            int clnt_sock = accept(serve_sock, NULL, NULL);
            if (clnt_sock == -1) ;
            else {
                if (!manage_fds(clnt_sock))
                    close_fd(clnt_sock);
            }
            char init_msg[40];
            strcpy(init_msg, "Anonymous FTP server ready.");
            send_response(clnt_sock, 220, init_msg);
            if (--ready_idx <= 0) continue;
        }
        for(int i = 0; i <= max_idx; i++) {
            int clnt_sock_tmp = clients[i].connect_serve_sock;
            if (clnt_sock_tmp == -1) continue;
            if (FD_ISSET(clnt_sock_tmp, &fd_read_set)) {
                if (recv_from_client(clnt_sock_tmp, i) == 0) {
                   // TODO:关闭连接
                   // 关闭控制
                   close_fd(i);
                   clients[i].transfer_serve_sock = -1;
                   return 0;
                }
                if (--ready_idx <= 0) continue;
                clnt_sock_tmp = clients[i].transfer_serve_sock;
                if(clnt_sock_tmp == -1) {
                    // 接收到 PORT 和 PASV命令后会修改 transfer_serve_sock
                    continue;
                }
                int mode = clients[i].mode;
                if (mode == PASV_MODE) {
                    int trans_sock = accept(clnt_sock_tmp, NULL, NULL);
                    if (trans_sock == -1) {
                        send_response(clients[i].connect_serve_sock, 425, NULL);
                        int trans_sock_tmp = clients[i].transfer_serve_sock;
                        if (trans_sock_tmp != -1) {
                            close(trans_sock_tmp);
                            clients[i].transfer_serve_sock = -1;
                            clients[i].state = LOG_IN;
                            clients[i].mode = NO_CONNECTION;
                            clients[i].offset = 0;
                            FD_CLR(trans_sock_tmp, &handle_set);
                        }
                        continue;
                    }
                    int trans_sock_tmp = clients[i].transfer_serve_sock;
                    int offset = clients[i].offset;
                    if (trans_sock_tmp != -1) {
                        close (trans_sock_tmp);
                        clients[i].transfer_serve_sock = -1;
                        clients[i].state = LOG_IN;
                        clients[i].mode = NO_CONNECTION;
                        FD_CLR(trans_sock_tmp, &handle_set);
                    }
                        clients[i].transfer_serve_sock = trans_sock;
                        FD_SET(trans_sock, &handle_set);
                        max_serve_sock = max(trans_sock, max_serve_sock);
                        clients[i].offset = offset;
                        clients[i].mode = READY;
                        clients[i].state = TRANSFER;
                }
                if (mode == PORT_MODE)  clients[i].mode = READY;
                if (mode == READY) {
                    // upload：上传到server
                    if (clients[i].rw_state == WRITE) upload(i);
                    if (clients[i].rw_state == READ) {
                        char sentence[8];
                        int len = recv(clnt_sock_tmp, sentence, 8, MSG_DONTWAIT);
                        if (len <= 0)
                        {
                            close_transfer_fd(i);
                            send_response(clients[i].connect_serve_sock, 426, NULL);
                            continue;
                        }
                    }
                }
                if (--ready_idx <= 0) continue;
            }
             else if(FD_ISSET(clnt_sock_tmp, &fd_write_set)) {
                int mode = clients[i].mode;
                if (mode == READY) {
                    if(clients[i].rw_state == READ) download(i);
                    else if(clients[i].rw_state == WRITE) upload(i);
                    else if(clients[i].rw_state == LIST) resp_list(i, clients[i].filename);
                }
                if (--ready_idx <= 0) continue;
            }
        }
    }
    close(serve_sock);
    return 0;
}
