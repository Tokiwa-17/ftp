import socket
import sys
from utils import *
from login import Ui_login
from cmd import Ui_cmd
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication
from FTPclient import FTPClient

if __name__ == '__main__':
    app = QApplication(sys.argv)
    client = FTPClient()
    sys.exit(app.exec_())