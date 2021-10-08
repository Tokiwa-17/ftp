#include "utils.h"
#include "cmd_handle.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>



int main(){
    //login();
    //PORT("192 168 44 133 12 34", 1);
    //PASV(NULL, 1);
    //SYST(NULL, 1);
    //TYPE("I", 1);
    //QUIT(NULL, 1);
    //ABOR(NULL, 1);
    RETR("1.txt", 1);
    return 0;
}
