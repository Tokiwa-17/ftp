#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

void login() {
    int serve_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));  //每个字节都用0填充
    serv_addr.sin_family = AF_INET;  //使用IPv4地址
    serv_addr.sin_addr.s_addr = inet_addr("192.168.44.131");  //具体的IP地址
    serv_addr.sin_port = htons(1234); 
    bind(serve_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(serve_sock, 10);
    printf("Wait for Connection..................\n");
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serve_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    char buf[1024];
    char check_str[] = "USER anonymous";
    char greetings[] = "You have logged in!\0";

    // 1. respond with an initial message.
    strcpy(buf, "220 Anonymous FTP server ready.\r\n");
    send(clnt_sock, buf, sizeof(buf) - 1, 0);
    // 2. client to send a “USER anonymous” command.
    recv(clnt_sock, buf, sizeof(buf) - 1, 0);
    //printf("RtnMsg: %s\n", buf);
    if(strcmp(buf, check_str) != 0) {
        printf("返回字符串不正确\n");
    }
    // 3. ask for an email address as a password
    strcpy(buf, "PASS some_password");
    send(clnt_sock, buf, sizeof(buf) - 1, 0);
    // 5. log in and send greetings
    recv(clnt_sock, buf, sizeof(buf) - 1, 0);
    strcpy(buf, greetings);
    send(clnt_sock, buf, sizeof(buf) - 1, 0);
    // TODO:标记用户状态为登录
    close(serve_sock);
    return;
}