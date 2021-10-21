import socket
import sys
import os
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QMessageBox, QTreeWidgetItem, QTreeWidget, QDirModel, QFileSystemModel, QInputDialog, QLabel
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
        #self.cmd.cmd_window.PORT_radioButton.clicked.connect(self.port_mode)
        #self.cmd.cmd_window.PASV_radioButton.clicked.connect(self.pasv_mode)
        #self.cmd.cmd_window.mkdir_pushButton.clicked.connect(self.mkdir)
        #self.cmd.cmd_window.rmdir_pushButton.clicked.connect(self.rmdir)
        self.window.cmd_window.radioButton_port.clicked.connect(self.port_mode)
        self.window.cmd_window.radioButton_pasv.clicked.connect(self.pasv_mode)
        #self.window.cmd_window.pushButton_connect.clicked.connect(self.new_connection)
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
        self.window.cmd_window.pushButton_local_MKDIR.clicked.connect(self.handle_local_dialog)
        self.window.cmd_window.pushButton_local_upload.clicked.connect(self.handle_local_dialog)
        self.window.cmd_window.pushButton_local_rename.clicked.connect(self.handle_local_dialog)
        self.window.cmd_window.pushButton_local_del.clicked.connect(self.handle_local_dialog)
        self.window.cmd_window.pushButton_remote_MKDIR.clicked.connect(self.handle_remote_dialog)
        self.window.cmd_window.pushButton_remote_download.clicked.connect(self.handle_remote_dialog)
        self.window.cmd_window.pushButton_remote_rename.clicked.connect(self.handle_remote_dialog)
        self.window.cmd_window.pushButton_remote_del.clicked.connect(self.handle_remote_dialog)
        self.remote_dir = {}
        self.window.cmd_window.progressBar_upload.setFormat("%v")
        self.window.cmd_window.progressBar_upload.reset()
        self.window.cmd_window.progressBar_download.reset()
        self.window.cmd_window.progressBar_download.setFormat("%v")

        #l = QLabel('test')
        #self.window.cmd_window.tab_finish.layout.addWidget(l)


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
        #self.client.pasv()
        self.show_remote_dir()
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
        #self.show_remote_dir()

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
        self.remote_dir = {}
        if self.client.mode == 'PASV':
            self.client.pasv()
        else:
            self.client.port()
        path = self.window.cmd_window.lineEdit_remote_site.text().strip()
        self.window.cmd_window.treeWidget_remote_site.clear()
        if path[0:4] == '/tmp':
            path = path[4:]
        file_list = self.client.list(path)
        #print(f'file_list: {file_list}')
        #if file_list is None:
        #    self.cmd.cmd_window.PASV_radioButton.setChecked(True)
        #    self.client.pasv()
        #    file_list = self.client.list(None)
        file_list = file_list.split('\n')[:-1]
        for file in file_list:
            file = file.split()
            if len(file[5]) == 3:
                date = file[5][:2] + '/' + file[6] + ' ' + file[7]
            else:
                date = '0' + file[5][:1] + '/' + file[6] + ' ' + file[7]
            if len(file) == 9:
                file_name = file[-1]
            else:
                file_name = ' '.join(file[8:])
            try:
                suffix = file_name.split('.')[1]
                item = QTreeWidgetItem([date, 'file', file[4], file_name])
                self.window.cmd_window.treeWidget_remote_site.addTopLevelItem(item)
                self.remote_dir[file_name] = int(file[4])
            except:
                item = QTreeWidgetItem([date, 'DIR', '/', file_name])
                self.window.cmd_window.treeWidget_remote_site.addTopLevelItem(item)
                self.remote_dir[file_name] = 0



    def handle_local_dialog(self):
        sender = self.sender()
        if sender == self.window.cmd_window.pushButton_local_MKDIR:
            dir_name, ok = QInputDialog.getText(self, '新建目录', '请输入目录名：')
            if ok:
                self.mkdir_local(dir_name)

        elif sender == self.window.cmd_window.pushButton_local_upload:
            src_filename, ok = QInputDialog.getText(self, '上传文件', '请输入源文件名：')
            if ok:
                dest_filename, ok = QInputDialog.getText(self, '上传文件', '请输入目标文件名：')
                if ok:
                    self.upload_local(src_filename, dest_filename)

        elif sender == self.window.cmd_window.pushButton_local_rename:
            filename, ok = QInputDialog.getText(self, '重命名文件', '请输入文件名')
            if ok:
                new_name, ok = QInputDialog.getText(self, '新文件名', '请输入新文件名')
                if ok:
                    self.rename_local(filename, new_name)

        elif sender == self.window.cmd_window.pushButton_local_del:
            filename, ok = QInputDialog.getText(self, '删除文件', '请输入文件名')
            if ok:
                self.rmdir_local(filename)

    def mkdir_local(self, dir_name):
        path = self.window.cmd_window.lineEdit_local_site.text()
        if path[-1] == '\\':
            path = path[:-1]
        if os.path.exists(path + '\\' + dir_name):
            pass
        os.makedirs(path + '\\' + dir_name)
        self.show_local_dir()

    def rmdir_local(self, filename):
        path = self.window.cmd_window.lineEdit_local_site.text()
        if path[-1] == '\\':
            path = path[:-1]
        if not os.path.exists(path + '\\' + filename):
            pass
        try:
            os.remove(path + '\\' + filename)
            self.show_local_dir()
        except:
            try:
                os.removedirs(path + '\\' + filename)
                self.show_local_dir()
            except:
                QMessageBox.information(None, 'Error', 'local Del', QMessageBox.Yes)
                return

    def upload_local(self, src_filename, dest_filename):
        if self.client.mode == 'PASV':
            self.client.pasv()
        if self.client.mode == 'PORT':
            self.client.port()
        src_path = self.window.cmd_window.lineEdit_local_site.text()
        if src_path[-1] == '\\':
            src_path = src_path[:-1]
        if not os.path.exists(src_path + '\\' + src_filename):
            return
        dest_path = self.window.cmd_window.lineEdit_remote_site.text().strip()
        if dest_path[-1] == '/':
            dest_path = dest_path[:-1]
        try:
            size = self.remote_dir[dest_filename]
        except:
            return

        self.client.stor(src_path + '\\' + src_filename, dest_path + '/' + dest_filename, self.window.cmd_window.progressBar_upload, 0)
        self.show_remote_dir()

    def rename_local(self, filename, new_name):
        path = self.window.cmd_window.lineEdit_local_site.text()
        if path[-1] == '\\':
            path = path[:-1]
        if not os.path.exists(path + '\\' + filename):
            return
        os.rename(path + '\\' + filename, path + '\\' + new_name)
        self.show_local_dir()

    def handle_remote_dialog(self):
        sender = self.sender()
        if sender == self.window.cmd_window.pushButton_remote_MKDIR:
            dir_name, ok = QInputDialog.getText(self, '新建目录', '请输入目录名：')
            if ok:
                self.mkdir_remote(dir_name)

        elif sender == self.window.cmd_window.pushButton_remote_download:
            filename, ok = QInputDialog.getText(self, '下载文件', '请输入文件名：')
            if ok:
                self.download_remote(filename)

        elif sender == self.window.cmd_window.pushButton_remote_rename:
            filename, ok = QInputDialog.getText(self, '重命名文件', '请输入文件名')
            if ok:
                new_name, ok = QInputDialog.getText(self, '新文件名', '请输入新文件名')
                if ok:
                    self.rename_remote(filename, new_name)

        elif sender == self.window.cmd_window.pushButton_remote_del:
            filename, ok = QInputDialog.getText(self, '删除文件', '请输入文件名')
            if ok:
                self.rmdir_remote(filename)

    def mkdir_remote(self, dir_name):
        path = self.window.cmd_window.lineEdit_remote_site.text()
        if path[-1] == '/':
            path = path[:-1]
        self.client.mkd(path + '/' + dir_name)
        self.show_remote_dir()
        self.remote_dir[dir_name] = 0

    def rmdir_remote(self, file_name):
        path = self.window.cmd_window.lineEdit_remote_site.text()
        if path[-1] == '/':
            path = path[:-1]
        self.client.rmd(path + '/' + file_name)
        self.show_remote_dir()
        try:
            del(self.remote_dir[file_name])
        except:
            pass

    def rename_remote(self, filename, new_name):
        path = self.window.cmd_window.lineEdit_remote_site.text()
        if path[-1] == '/':
            path = path[:-1]
        self.client.rnfr(path + '/' + filename)
        self.client.rnto(path + '/' + new_name)
        self.show_remote_dir()
        try:
            size = self.remote_dir[filename]
            del(self.remote_dir[filename])
            self.remote_dir[new_name] = size
        except:
            pass

    def download_remote(self, filename):
        if self.client.mode == 'PASV':
            print(self.client.pasv())
        if self.client.mode == 'PORT':
            print(self.client.port())

        try:
            size = self.remote_dir[filename]
            if int(size) == 0:
                return
        except:
            size = 0
            return
        print(f'size: {size}')
        self.client.retr(filename, filename, int(size), 0, self.window.cmd_window.progressBar_download)
        self.show_local_dir()

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

