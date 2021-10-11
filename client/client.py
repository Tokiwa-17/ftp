import socket
import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow
from login import Ui_login

class Login(Ui_login, QMainWindow):
    def __init__(self):
        super().__init__()
        self.login_window = Ui_login()
        self.login_window.setupUi(self)
        self.login_window.username_lineEdit.setText('anonymous')
        self.login_window.password_lineEdit.setText('yuanlf18@mails.tsinghua.edu.cn')
        self.login_window.ip_lineEdit.setText('192.168.44.133')
        self.login_window.port_lineEdit.setText('21')


class Client(QMainWindow):
    def __init__(self):
        super().__init__()
        self.user_status = 0 # login required
        self.login_window = Login()
        self.login_window.login_window.login_pushButton.clicked.connect(self.login)
        self.login_window.show()

    def login(self):
        try:
            self.s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.s.connect((self.login_window.login_window.ip_lineEdit.text(), int(self.login_window.login_window.port_lineEdit.text())))
            #print('socket establish success!')
        except socket.error as msg:
            print(msg)
            print(sys.exit(1))

        user_cmd = "USER anonymous\r\n"
        self.s.send(user_cmd.encode('utf-8'))
        resp_msg = (self.s.recv(100).split())[0].decode('utf-8')
        if(resp_msg != '331'):
            print('USER command response error!')
            sys.exit(1)

        pass_cmd = "PASS " +  self.login_window.login_window.password_lineEdit.text() + "\r\n"
        self.s.send(pass_cmd.encode('utf-8'))
        resp_msg = (self.s.recv(100).split())[0].decode('utf-8')
        if(resp_msg != '230'):
            print('PASS command response error')
            sys.exit(1)
        self.user_status = 1
        print("LOGIN SUCCESS!")
        self.login_window.close()