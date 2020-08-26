try:
    from PyQt5.QtGui import *
    from PyQt5.QtCore import *
    from PyQt5.QtWidgets import *
except ImportError:
    print("Please Use PyQt5!")


import time

class AlertDialog(QDialog):

    def __init__(self, parent=None):
        super(AlertDialog, self).__init__(parent)
        self.setWindowFlags(Qt.Window | Qt.WindowTitleHint | Qt.CustomizeWindowHint)
        smallfont = QFont("Roboto", 8)
        largefont = QFont("Roboto", 10)
        largefont.setBold(True)
        self.setWindowTitle("Success!")
        self.button = QPushButton()
        pixmap = QPixmap("resources/icons/success.png")
        self.dummyicon = QLabel()
        self.dummyicon.setMaximumSize(5, 5)
        self.icon = QLabel()
        self.icon.setPixmap(pixmap)
        self.icon.setScaledContents(True)
        self.icon.setMaximumSize(70, 70)
        self.button.setText("Ok, Got it!")
        self.button.setMaximumSize(100,25)
        self.imperative = QLabel()
        self.imperative.setGeometry(QRect(0, 0, 200, 10))
        self.imperative.setAlignment(Qt.AlignCenter)
        self.imperative.setFont(largefont)
        self.suggestion = QLabel()
        self.suggestion.setGeometry(QRect(0, 0, 200, 10))
        self.suggestion.setAlignment(Qt.AlignCenter)
        self.suggestion.setFont(smallfont)
        self.dummy = QPushButton()
        self.dummy.setVisible(False)
        layout = QHBoxLayout() 
        layout.addWidget(self.dummy)
        layout.addWidget(self.button)
        layout_ = QVBoxLayout()
        layout_.addWidget(self.imperative)
        layout_.addWidget(self.suggestion)
        _layout = QHBoxLayout()
        _layout.addWidget(self.dummyicon)
        _layout.addWidget(self.icon)
        _layout.addLayout(layout_)
        parent_layout = QVBoxLayout()
        parent_layout.addLayout(_layout)
        parent_layout.addLayout(layout)
        self.icon.setScaledContents(True)
        self.setLayout(parent_layout)
        self.button.clicked.connect(self.action)
        self.setFixedSize(300,150)

    def setStatus(self, status):
        if(status == 'image'):
            self.imperative.setText("Success!!")
            self.suggestion.setText("Image Saved Successfully!")
        elif(status == 'video'):
            self.imperative.setText("Success!")
            self.suggestion.setText("Video Saved Successfully!")
        else:
            print('Status not Valid')

    def action(self):
        self.reject()
