# pragma once

int check_ipaddr(int);

int check_port_invalid(int);

void send_test(int serve_sock, char *buf);

void send_response(int serve_sock, int code, char *resp);

void get_absolute_path(char *prefix, char *src, char *dest);