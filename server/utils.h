# pragma once

int max(int a, int b);

int check_ipaddr(int);

int check_port_invalid(int);

void send_test(int serve_sock, char *buf);

void send_response(int clnt_sock, int code, char *resp_msg);

int recv_from_client(int clnt_sock, int idx);

void cmd_handler(char *cmd, char *param, int idx);

int transfer(char *param, int idx);

int safe_recv(int fd, char *buf, int len);

int safe_send(int fd, char *buf, int len);

void upload(int idx);

void download(int idx);

void resp_list(int idx, char *dest);