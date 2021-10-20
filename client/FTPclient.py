import socket
import sys
import os
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QMessageBox, QTreeWidgetItem, QTreeWidget, QDirModel, QFileSystemModel
from login import Ui_login
from PyQt5.QtCore import QDir
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
        self.local_path = os.getcwd()
        self.window.cmd_window.lineEdit_local_site.setText(self.local_path)
        self.window.cmd_window.lineEdit_remote_site.setText('/tmp')
        self.window.cmd_window.pushButton_local_site.clicked.connect(self.show_local_dir)
        self.window.cmd_window.pushButton_remote_site.clicked.connect(self.show_remote_dir)
        self.window.cmd_window.treeWidget_local_site.setColumnCount(4)
        self.window.cmd_window.treeWidget_local_site.setHeaderLabels(['Last modified', 'Type', 'Size', 'Name'])
        self.window.cmd_window.treeWidget_remote_site.setColumnCount(4)
        self.window.cmd_window.treeWidget_remote_site.setHeaderLabels(['Last modified', 'Type', 'Size', 'Name'])
        self.show_local_dir()

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
        self.show_remote_dir()

    def show_local_dir(self):
        path = self.window.cmd_window.lineEdit_local_site.text()
        #print(path)
        self.window.cmd_window.treeWidget_local_site.clear()
        if not os.system('cd ' + path):
            #dir = os.system('dir ' + path)
            dir = os.popen('dir ' + path)
            dir = dir.readlines()[7:-2]
            dir = [file[:-1] for file in dir]
            self.local_path = path
        else:
            dir = None
        if dir:
            #print(dir)
            for file in dir:
                file_info = file.split()
                if file_info[2] == '<DIR>':
                    item = QTreeWidgetItem([file_info[0] + ' ' + file_info[1], 'DIR', '/', file_info[-1]])
                    self.window.cmd_window.treeWidget_local_site.addTopLevelItem(item)
                else:
                    item = QTreeWidgetItem([file_info[0] + ' ' + file_info[1], 'file', file_info[2], file_info[-1]])
                    self.window.cmd_window.treeWidget_local_site.addTopLevelItem(item)


    def show_remote_dir(self, dir=None):
        path = self.window.cmd_window.lineEdit_remote_site.text()
        self.window.cmd_window.treeWidget_remote_site.clear()
        if path[0:4] == '/tmp':
            path = path[4:]
        file_list = self.client.list(path)
        #print(f'file_list: {file_list}')
        if file_list is None:
            self.cmd.cmd_window.PASV_radioButton.setChecked(True)
            self.client.pasv()
            file_list = self.client.list(None)
        file_list = file_list.split('\n')[:-1]
        for file in file_list:
            file = file.split()
            if file[4] == '0':
                item = QTreeWidgetItem([file[5][:2] + '/' + file[6] + ' ' + file[7], 'DIR', '/', file[-1]])
                self.window.cmd_window.treeWidget_remote_site.addTopLevelItem(item)
            else:
                item = QTreeWidgetItem([file[5][:2] + '/' + file[6] + ' ' + file[7], 'file', file[4], file[-1]])
                self.window.cmd_window.treeWidget_remote_site.addTopLevelItem(item)


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

