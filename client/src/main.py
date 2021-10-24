import socket
import sys
import os
from qt_material import apply_stylesheet
from utils import *
from login import Ui_login
#from cmd import Ui_cmd
from PyQt5 import QtCore, QtGui, QtWidgets
from PyQt5.QtWidgets import QApplication
from FTPclient import FTPClient
from global_ import Global

if __name__ == '__main__':
    app = QApplication(sys.argv)
    apply_stylesheet(app, theme='dark_blue.xml')
    client = FTPClient()
    sys.exit(app.exec_())
    os.system('pause')