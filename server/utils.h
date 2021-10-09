# pragma once

int check_ipaddr(int);

int check_port_invalid(int);

void send_test(int serve_sock, char *buf);

void send_response(int clnt_sock, int code, char *resp_msg);

void get_absolute_path(char *prefix, char *src, char *dest);

int recv_from_client(int clnt_sock, int idx);

void cmd_handler(char *cmd, char *param, int idx);
