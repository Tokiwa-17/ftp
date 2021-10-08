#include "utils.h"
#include "cmd_handle.h"
#include "socket_utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

extern int listen_port;

int main(){

    listen_port = 21;
    int serve_sock = generate_sock(listen_port);
    if(serve_sock == -1) return 0;
    
    //login();
    //PORT("192 168 44 133 12 34", 1);
    //PASV(NULL, 1);
    //SYST(NULL, 1);
    //TYPE("I", 1);
    //QUIT(NULL, 1);
    //ABOR(NULL, 1);
    //RETR("1.txt", 1);
    return 0;
}
