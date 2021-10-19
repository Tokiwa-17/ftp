# pragma once

int generate_sock(int port);

void sock_init(int sock);

int manage_fds(int cur_fd);

void close_fd(int idx);

void close_transfer_fd(int idx);

void update_trans_sock(int tr_sock, int idx);

int get_local_IPaddr();