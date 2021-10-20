import socket
import sys
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QMessageBox, QTreeWidgetItem, QTreeWidget
from login import Ui_login
from cmd import Ui_cmd
from client import Client
from window import Ui_window
from global_ import Global

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

class Window(Ui_window, QMainWindow):
    def __init__(self):
        super().__init__()
        self.cmd_window = Ui_window()
        self.cmd_window.setupUi(self)


class FTPClient(QMainWindow):
    def __init__(self):
        super().__init__()
        self.client = Client(self)
        self.user_status = 0 # login required
        self.login_status = False
        self.login = Login()
        self.login.login_window.login_pushButton.clicked.connect(self.Login)
        self.login.show()
        self.cmd = Cmd()
        self.window = Window()
        self.client.mode = 'PASV'
        self.cmd.cmd_window.PORT_radioButton.clicked.connect(self.port_mode)
        self.cmd.cmd_window.PASV_radioButton.clicked.connect(self.pasv_mode)
        self.cmd.cmd_window.mkdir_pushButton.clicked.connect(self.mkdir)
        self.cmd.cmd_window.rmdir_pushButton.clicked.connect(self.rmdir)
        self.window.cmd_window.radioButton_port.clicked.connect(self.port_mode)
        self.window.cmd_window.radioButton_pasv.clicked.connect(self.pasv_mode)
        self.window.cmd_window.pushButton_connect.clicked.connect(self.new_connection)
        self.window.cmd_window.lineEdit_local_site.setText('/')
        self.window.cmd_window.lineEdit_remote_site.setText('/tmp')

    def Login(self):
        try:
            self.client.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client.socket.connect((self.login.login_window.ip_lineEdit.text(), int(self.login.login_window.port_lineEdit.text())))
            #print('socket establish success!')
            self.client.recv_msg()
            #self.window.cmd_window.textEdit_cmd.append(self.client.recv_message)
        except socket.error as msg:
            print(msg)
            print(sys.exit(1))
        if self.client.code != 220:
            return False
        user_cmd = "USER anonymous"
        self.client.send_msg(user_cmd)
        self.client.recv_msg()
        #self.window.cmd_window.textEdit_cmd.append(self.client.recv_message)
        if self.client.code != 331:
            return False
        pass_cmd = "PASS " + self.login.login_window.password_lineEdit.text()
        self.client.send_msg(pass_cmd)
        self.client.recv_msg()
        #self.window.cmd_window.textEdit_cmd.append(self.client.recv_message)
        if self.client.code != 230:
            return False
        self.login_status = True
        self.login.close()
        #self.cmd.show()
        self.window.cmd_window.lineEdit_host.setText('127.0.0.1')
        self.window.cmd_window.lineEdit_user.setText('anonymous')
        self.window.cmd_window.lineEdit_port.setText('21')
        self.window.cmd_window.radioButton_pasv.setChecked(True)
        self.window.show()
        """
        TEST
        """
        #self.client.cwd('/test')
        #self.client.rnfr('test.txt')
        #self.client.rnto('tttt.txt')
        #self.client.retr('test.txt', 'local_test.txt', 72, 0, self.cmd.cmd_window.download_progressBar)
        #self.client.stor('local_test.txt', 'test.txt', self.cmd.cmd_window.upload_progressBar, 0)
        #print(self.client.list(None))
        #self.display_list()
        #self.client.pwd()
        """
        TEST
        """
        return True

    def pasv_mode(self):
        self.client.mode = 'PASV'
        self.cmd.cmd_window.PASV_radioButton.setChecked(True)

    def port_mode(self):
        self.client.mode = 'PORT'
        self.cmd.cmd_window.PORT_radioButton.setChecked(True)

    def new_connection(self):
        print(f'mode: {self.client.mode}')
        if self.client.mode == 'PASV':
            if self.client.pasv():
                QMessageBox.information(self, "connection", "连接成功")
            else:
                QMessageBox.information(self, "connection", "连接失败")
        else:
            if self.client.port():
                QMessageBox.information(self, "connection", "连接成功")
            else:
                QMessageBox.information(self, "connection", "连接失败")



    def mkdir(self):
        pass

    def rmdir(self):
        pass

    def display_list(self, dir=None):
        root = self.cmd.cmd_window.file_list_treeWidget
        file_list = self.client.list(None)
        print(f'file_list: {file_list}')
        if file_list is None:
            self.client.mode == 'PASV'
            self.cmd.cmd_window.PASV_radioButton.setChecked(True)
            self.cmd.cmd_window.PORT_radioButton.setChecked(False)
            file_list = self.client.list(None)
        # TODO

        if file_list == '' or file_list is None:
            return []

        file_list = file_list.split('\n')[:-1]
        items = []
        for file in file_list:
            item = QTreeWidgetItem(root)
            item.setText(0, file)
            #root.addChild(item)
            items.append(item)
        return items

