import socket
import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QMessageBox
from login import Ui_login
from cmd import Ui_cmd
from client import Client

class Login(Ui_login, QMainWindow):
    def __init__(self):
        super().__init__()
        self.login_window = Ui_login()
        self.login_window.setupUi(self)
        self.login_window.username_lineEdit.setText('anonymous')
        self.login_window.password_lineEdit.setText('yuanlf18@mails.tsinghua.edu.cn')
        self.login_window.ip_lineEdit.setText('192.168.44.133')
        self.login_window.port_lineEdit.setText('21')



class Cmd(Ui_cmd, QMainWindow):
    def __init__(self):
        super().__init__()
        self.cmd_window = Ui_cmd()
        self.cmd_window.setupUi(self)


class FTPClient(QMainWindow):
    def __init__(self):
        super().__init__()
        self.client = Client()
        self.user_status = 0 # login required
        self.login_status = False
        self.login = Login()
        self.login.login_window.login_pushButton.clicked.connect(self.Login)
        self.login.show()
        self.cmd = Cmd()
        self.transfer_mode = 'PASV'
        self.cmd.cmd_window.PORT_radioButton.clicked.connect(self.port_mode)
        self.cmd.cmd_window.PASV_radioButton.clicked.connect(self.pasv_mode)
        self.cmd.cmd_window.mkdir_pushButton.clicked.connect(self.mkdir)
        self.cmd.cmd_window.rmdir_pushButton.clicked.connect(self.rmdir)

    def Login(self):
        try:
            self.client.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client.socket.connect((self.login.login_window.ip_lineEdit.text(), int(self.login.login_window.port_lineEdit.text())))
            print('socket establish success!')
        except socket.error as msg:
            print(msg)
            print(sys.exit(1))

        user_cmd = "USER anonymous"
        self.client.send_msg(user_cmd)
        self.client.recv_msg()
        if self.client.code != 331:
            return False
        pass_cmd = "PASS " + self.login.login_window.password_lineEdit.text()
        self.client.send_msg(pass_cmd)
        code = self.client.recv_msg()
        if self.client.code != 230:
            return False
        self.login_status = True
        self.login.close()
        self.cmd.show()
        """
        TEST
        """
        self.client.pasv()

        # self.client.retr('test.txt', 'local_test.txt', 72, 0, self.cmd.cmd_window.download_progressBar)
        self.client.stor('local_test.txt', 'test.txt', self.cmd.cmd_window.upload_progressBar, 0)
        """
        TEST
        """
        return True

    def pasv_mode(self):
        self.transfer_mode = 'PASV'
        self.cmd.cmd_window.PASV_radioButton.setChecked(True)
        self.cmd.cmd_window.PORT_radioButton.setChecked(False)

    def port_mode(self):
        self.transfer_mode = 'PORT'
        self.cmd.cmd_window.PASV_radioButton.setChecked(False)
        self.cmd.cmd_window.PORT_radioButton.setChecked(True)

    def mkdir(self):
        pass

    def rmdir(self):
        pass