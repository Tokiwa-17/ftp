import socket
import sys
import random
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QMainWindow, QMessageBox
from login import Ui_login
from cmd import Ui_cmd

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
    def __init__(self):
        super().__init__()
        self.host = None
        self.mode = None
        self.code = None
        self.socket = None
        self.data_socket = None
        self.data_address = None
        self.localIP = get_local_IP()


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

    def pasv(self):
        self.send_msg("PASV")
        resp_msg = self.recv_msg()
        if self.code != 227:
            return False
        print(resp_msg)
        self.data_address = resp_msg.split('(')[1][0:-1]
        self.data_address = self.data_address.split(',')
        ip_address = '.'.join(self.data_address[0:4])
        port = int(self.data_address[-2]) * 256 + int(self.data_address[-1])
        self.data_address = ip_address + ' ' + str(port)
        return True

    def port(self):
        port = get_aval_port(self.localIP)
        port_cmd = 'PORT ' + self.localIP.replace('.', ',') + ',' + \
            str(port // 256) + ',' + str(port % 256)
        self.send_msg(port_cmd)
        self.recv_msg()
        if self.code != 200:
            return False
        print(port)
        self.data_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.data_socket.bind((self.localIP, port))
        self.data_socket.listen(5)
        return True








