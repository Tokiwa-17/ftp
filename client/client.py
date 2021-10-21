import socket
import sys
import os
import random
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QMessageBox
from login import Ui_login
from cmd import Ui_cmd
from utils import *
from global_ import Global

def get_local_IP():
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(('8.8.8.8', 80))
        ip = s.getsockname()[0]
    finally:
        s.close()
    return ip

def get_aval_port(ip):
    while True:
        port = random.randint(20000, 65535)
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        try:
            s.bind((ip, port))
        except:
            continue
        s.close()
        return port

class Client():
    def __init__(self, ftp):
        super().__init__()
        self.host = None
        self.mode = 'PASV'
        self.code = None
        self.socket = None
        self.data_socket = None
        self.data_address = None
        self.localIP = get_local_IP()
        self.download_thread = None
        self.upload_thread = None
        self.prefix = None
        self.recv_message = None
        self.ftp = ftp


    def send_msg(self, cmd):
        cmd += '\r\n'
        try:
            self.socket.send(cmd.encode('utf-8'))
        except Exception as e:
            pass

    def recv_msg(self):
        try:
            resp = self.socket.recv(1024).decode()
            code_line = resp.split('\r\n')[-2]
            self.code = int(code_line.split(' ')[0])
            self.recv_message = code_line
            self.ftp.window.cmd_window.textEdit_cmd.append(code_line)
            return code_line
        except:
            pass

    def port_mode(self):
        self.mode = 'PORT'

    def pasv_mode(self):
        self.mode = 'PASV'

    def syst(self):
        self.send_msg("SYST")
        self.recv_msg()

    def type(self):
        self.send_msg("TYPE I")
        self.recv_msg()

    def quit(self):
        self.send_msg("QUIT")
        self.recv_msg()
        self.socket.close()

    def abor(self):
        self.send_msg('ABOR')
        self.recv_msg()

    """
    请求服务器DTP在一个数据端口上监听并等待连接而不是收到传输命令后主动发起连接
    """
    def pasv(self):
        self.send_msg("PASV")
        resp_msg = self.recv_msg()
        if self.code != 227:
            return False
        #print(resp_msg)
        self.data_address = resp_msg.split('(')[1][0:-1]
        self.data_address = self.data_address.split(',')
        ip_address = '.'.join(self.data_address[0:4])
        port = int(self.data_address[-2]) * 256 + int(self.data_address[-1])
        self.data_address = ip_address, port
        return True

    """
    这个参数用来指定数据连接时的主机数据端口
    """
    def port(self):
        self.mode = 'PORT'
        port = get_aval_port(self.localIP)
        port_cmd = 'PORT ' + self.localIP.replace('.', ',') + ',' + \
            str(port // 256) + ',' + str(port % 256)
        self.send_msg(port_cmd)
        resp_msg = self.recv_msg()
        #print(resp_msg)
        if self.code != 200:
            return False
        #print(port)
        self.data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.data_socket.bind((self.localIP, port))
        self.data_socket.listen(5)
        return True

    """
    server -> client
        * src_path: file in server
        * dest_path: 
    """
    def retr(self, src_path, dest_path, filesize, offset, progress_bar):
        self.send_msg('RETR ' + src_path)
        print(self.recv_msg())
        if self.code != 150:
            self.data_socket.close()
            return
        if self.mode == 'PASV':
            self.data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                #print(self.data_address)
                self.data_socket.connect(self.data_address)
            except:
                print('PASV Error')
                return
        else:
            try:
                new_socket, _ = self.data_socket.accept()
                self.data_socket.close()
                self.data_socket = new_socket
            except Exception as e:
                return

        def update_progress_bar(progress):
            progress_bar.setValue(int(progress) * 100 / filesize)
        self.download_thread = DownloadHandler(self, filesize, offset, dest_path)
        self.download_thread.progress_bar_signal.connect(update_progress_bar)
        self.download_thread.run()

    """
    server接收数据
    src -> dest
    """
    def stor(self, src_path, dest_path, progress_bar, offset):
        filesize = os.path.getsize(src_path)
        self.send_msg('STOR ' + dest_path)
        if self.mode == 'PASV':
            self.data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                self.data_socket.connect(self.data_address)
            except:
                return

        else:
            try:
                new_socket, _ = self.data_socket.accept()
                self.data_socket.close()
                self.data_socket = new_socket
            except:
                return
        self.recv_msg()

        def update_progress_bar(progress):
            progress_bar.setValue(int(progress) * 100 / filesize)

        self.upload_thread = UploadHandler(self, filesize, offset, src_path)
        self.upload_thread.progress_bar_signal.connect(update_progress_bar)
        self.upload_thread.run()


    def mkd(self, folder_name):
        self.send_msg('MKD ' + folder_name)
        self.recv_msg()

    def rmd(self, folder_name):
        self.send_msg('RMD ' + folder_name)
        self.recv_msg()

    def rnfr(self, folder_name):
        self.send_msg('RNFR ' + folder_name)
        self.recv_msg()

    def rnto(self, folder_name):
        self.send_msg('RNTO ' + folder_name)
        self.recv_msg()

    def cwd(self, path):
        self.send_msg('CWD ' + path)
        self.recv_msg()

    def pwd(self):
        self.send_msg('PWD ')
        msg = self.recv_msg()
        self.prefix = msg.split(' ')[1].strip()[1:-1]
        print(self.prefix)

    def rest(self, offset):
        self.send_msg('REST ' + str(offset))
        self.recv_msg()


    def download_file(self, src_path, dest_path, filesize, offset, progress_bar):
        """
        dest_path: 下载文件的目标地址
        """
        if self.mode == 'PASV':
            if not self.pasv():
                QMessageBox.information(None, 'Error', 'PASV error', QMessageBox.Yes)
                return
        else:
            if not self.port():
                QMessageBox.information(None, 'Error', 'PORT error.', QMessageBox.Yes)
                return
        if offset:
            self.rest(offset)

        path = os.path.join(dest_path, os.path.basename(src_path))
        self.retr(src_path, path, filesize, offset, progress_bar)

    """
    LIST命令传送指定目录下的文件列表
    """
    def list(self, dest_path):
        if self.mode == 'PASV':
            pass
            #if not self.pasv():
            #    QMessageBox.information(None, 'Error', 'PASV error.', QMessageBox.Yes)
            #   return None
        else:
            pass
            #if not self.port():
            #    QMessageBox.information(None, 'Error', 'PORT error', QMessageBox.Yes)
            #    return None
        if dest_path:
            self.send_msg('LIST ' + dest_path)
        else:
            self.send_msg('LIST')

        if self.mode == 'PASV':
            self.data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            try:
                self.data_socket.connect(self.data_address)
            except:
                return None

        else:
            try:
                new_socket, _ = self.data_socket.accept()
                self.data_socket.close()
                self.data_socket = new_socket
            except:
                return None
        self.recv_msg()
        list = ''
        while True:
            data = self.data_socket.recv(4096).decode()
            list += data
            if not data:
                break
        self.recv_msg()
        self.data_socket.close()
        return list








