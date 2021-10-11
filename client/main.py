import socket
import sys
from utils import *
from login import Ui_login
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication
from client import Client

if __name__ == '__main__':
    app = QApplication(sys.argv)
    client = Client()
    sys.exit(app.exec_())
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect(('192.168.44.133', 21))
        print('suc')
    except socket.error as msg:
        print(msg)
        print(sys.exit(1))

    str_user = "USER anonymous\r\n"
    s.send(str_user.encode('utf-8'))
    print(s.recv(100))

    str_pass = "PASS yuanlf18@mails.tsinghua.edu.cn\r\n"
    s.send(str_pass.encode('utf-8'))
    print(s.recv(100))

    str_port = "PORT 192 168 44 133 12 34"
    s.send(str_port.encode('utf-8'))
    print(s.recv(100))

    str_pasv = "PASV"
    s.send(str_pasv.encode('utf-8'))
    print(s.recv(100))

    str_retr = "RETR 1.txt"
    s.send(str_retr.encode('utf-8'))
    print(s.recv(100))

    str_stor = "STOR 1.txt"
    s.send(str_stor.encode('utf-8'))
    print(s.recv(100))

    str_syst = "SYST"
    s.send(str_syst.encode('utf-8'))
    print(s.recv(100))

    str_type = "TYPE I"
    s.send(str_type.encode('utf-8'))
    print(s.recv(100))

    #str_quit = "QUIT"
    #s.send(str_quit.encode('utf-8'))
    #print(s.recv(100))

    #str_abor = "ABOR"
    #s.send(str_abor.encode('utf-8'))
    #print(s.recv(100))
    #print(s.recv(100))

    str_mkd = "MKD test"
    s.send(str_mkd.encode('utf-8'))
    print(s.recv(100))

    #str_cwd = "CWD test"
    #s.send(str_cwd.encode('utf-8'))
    #print(s.recv(100))

    str_pwd = "PWD"
    s.send(str_pwd.encode('utf-8'))
    print(s.recv(100))

    str_rmd = "RMD test"
    s.send(str_rmd.encode('utf-8'))
    print(s.recv(100))

    str_rnfr = "RNFR 1.txt"
    s.send(str_rnfr.encode('utf-8'))
    print(s.recv(100))

    str_rnto = "RNTO 2.txt"
    s.send(str_rnto.encode('utf-8'))
    print(s.recv(100))
    """

