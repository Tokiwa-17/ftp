# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'cmd.ui'
#
# Created by: PyQt5 UI code generator 5.15.2
#
# WARNING: Any manual changes made to this file will be lost when pyuic5 is
# run again.  Do not edit this file unless you know what you are doing.


from PyQt5 import QtCore, QtGui, QtWidgets


class Ui_cmd(object):
    def setupUi(self, cmd):
        cmd.setObjectName("cmd")
        cmd.resize(800, 632)
        self.centralwidget = QtWidgets.QWidget(cmd)
        self.centralwidget.setObjectName("centralwidget")
        self.verticalLayout = QtWidgets.QVBoxLayout(self.centralwidget)
        self.verticalLayout.setObjectName("verticalLayout")
        self.textBrowser = QtWidgets.QTextBrowser(self.centralwidget)
        self.textBrowser.setObjectName("textBrowser")
        self.verticalLayout.addWidget(self.textBrowser)
        self.file_list_treeWidget = QtWidgets.QTreeWidget(self.centralwidget)
        self.file_list_treeWidget.setObjectName("file_list_treeWidget")
        self.file_list_treeWidget.headerItem().setText(0, "1")
        self.verticalLayout.addWidget(self.file_list_treeWidget)
        self.horizontalLayout = QtWidgets.QHBoxLayout()
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.PORT_radioButton = QtWidgets.QRadioButton(self.centralwidget)
        self.PORT_radioButton.setObjectName("PORT_radioButton")
        self.horizontalLayout.addWidget(self.PORT_radioButton)
        self.PASV_radioButton = QtWidgets.QRadioButton(self.centralwidget)
        self.PASV_radioButton.setObjectName("PASV_radioButton")
        self.horizontalLayout.addWidget(self.PASV_radioButton)
        self.verticalLayout.addLayout(self.horizontalLayout)
        self.mkdir_pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.mkdir_pushButton.setObjectName("mkdir_pushButton")
        self.verticalLayout.addWidget(self.mkdir_pushButton)
        self.rmdir_pushButton = QtWidgets.QPushButton(self.centralwidget)
        self.rmdir_pushButton.setObjectName("rmdir_pushButton")
        self.verticalLayout.addWidget(self.rmdir_pushButton)
        self.horizontalLayout_3 = QtWidgets.QHBoxLayout()
        self.horizontalLayout_3.setObjectName("horizontalLayout_3")
        self.verticalLayout.addLayout(self.horizontalLayout_3)
        self.download_progressBar = QtWidgets.QProgressBar(self.centralwidget)
        self.download_progressBar.setProperty("value", 24)
        self.download_progressBar.setObjectName("download_progressBar")
        self.verticalLayout.addWidget(self.download_progressBar)
        self.upload_progressBar = QtWidgets.QProgressBar(self.centralwidget)
        self.upload_progressBar.setProperty("value", 24)
        self.upload_progressBar.setObjectName("upload_progressBar")
        self.verticalLayout.addWidget(self.upload_progressBar)
        cmd.setCentralWidget(self.centralwidget)
        self.menubar = QtWidgets.QMenuBar(cmd)
        self.menubar.setGeometry(QtCore.QRect(0, 0, 800, 26))
        self.menubar.setObjectName("menubar")
        cmd.setMenuBar(self.menubar)
        self.statusbar = QtWidgets.QStatusBar(cmd)
        self.statusbar.setObjectName("statusbar")
        cmd.setStatusBar(self.statusbar)

        self.retranslateUi(cmd)
        QtCore.QMetaObject.connectSlotsByName(cmd)

    def retranslateUi(self, cmd):
        _translate = QtCore.QCoreApplication.translate
        cmd.setWindowTitle(_translate("cmd", "MainWindow"))
        self.PORT_radioButton.setText(_translate("cmd", "PORT"))
        self.PASV_radioButton.setText(_translate("cmd", "PASV"))
        self.mkdir_pushButton.setText(_translate("cmd", "MKDIR"))
        self.rmdir_pushButton.setText(_translate("cmd", "RMDIR"))