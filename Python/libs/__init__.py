#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt5.QtCore import pyqtSignal, pyqtSlot, Qt, QThread
from PyQt5.QtWidgets import QWidget, QApplication, QLabel, QVBoxLayout
from PyQt5.QtGui import QPixmap

from .alertdialog import AlertDialog

__version_info__ = ('1', '0', '0')
__version__ = '.'.join(__version_info__)
