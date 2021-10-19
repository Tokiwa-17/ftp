import socket
import sys
from PyQt5.QtWidgets import *
from PyQt5.QtGui import *
from PyQt5.QtCore import *
import time

class DownloadHandler(QThread):
    progress_bar_signal = pyqtSignal(str)
    complete_signal = pyqtSignal()

    def __init__(self, ftp, size, offset, path):
        super().__init__()
        self.ftp = ftp
        self.size = size
        self.offset = offset
        self.path = path

    def run(self):
        if self.offset == 0:
            mode = 'wb'
        else:
            mode = 'ab'
        try:
            with open(self.path, mode) as f:
                while True:
                    data_stream = self.ftp.data_socket.recv(4096)
                    #print(f'len: {len(data_stream)}')
                    #time.sleep(0.01)
                    if not data_stream:
                        #print(f'data_stream: {data_stream}')
                        break
                    self.offset += len(data_stream)
                    #print(f'offset: {self.offset}')
                    self.progress_bar_signal.emit(str(self.offset))
                    f.write(data_stream)
        except:
            pass
        self.ftp.recv_msg()
        self.complete_signal.emit()
        self.exit()

class UploadHandler(QThread):
    progress_bar_signal = pyqtSignal(str)
    complete_signal = pyqtSignal()

    def __init__(self, ftp, size, offset, path):
        super().__init__()
        self.ftp = ftp
        self.size = size
        self.offset = offset
        self.path = path

    def run(self):
        try:
            progress = self.offset
            with open(self.path, 'rb') as f:
                f.seek(self.offset)
                while True:
                    data = f.read(4096)
                    if not data:
                        break
                    if self.ftp.data_socket.send(data) <= 0:
                        break
                    progress += len(data)
                    time.sleep(0.01)
                    self.progress_bar_signal.emit(str(progress))
            self.ftp.data_socket.close()
            self.ftp.recv_msg()
        except:
            self.ftp.recv_msg()
        self.complete_signal.emit()
        self.exit()