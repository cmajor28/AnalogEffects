import sys
from PySide.QtCore import *
from PySide.QtGui import *
from lcdWindow import Ui_MainWindow

class LCDWindow(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super(LCDWindow, self).__init__()
        self.setupUi(self)
        self.showMaximized()
        self.setWindowFlags(Qt.FramelessWindowHint)

if __name__ == '__main__':

    app = QApplication(sys.argv)
    window = LCDWindow()
    window.show()
    sys.exit(app.exec_())
