import sys
from PySide.QtCore import *
from PySide.QtGui import *
from .lcdWindow import Ui_MainWindow
from queue import Queue

class Invoker(QObject):
    def __init__(self):
        super(Invoker, self).__init__()
        self.queue = Queue()

    def invoke(self, func, *args):
        f = lambda: func(*args)
        self.queue.put(f)
        QMetaObject.invokeMethod(self, "handler", QtCore.Qt.QueuedConnection)

    @Slot()
    def handler(self):
        f = self.queue.get()
        f()

class LCDWindow(QMainWindow, Ui_MainWindow):

    def __init__(self, info, updateInfoCallback):
        super(LCDWindow, self).__init__()
        self.setupUi(self)
        self.showMaximized()
        self.setWindowFlags(Qt.FramelessWindowHint)

        self.updateInfo(info)

        self.updateInfoCallback = updateInfoCallback
        self.checkBoxPedalMode.clicked.connect(self.pedalModeChecked)
        self.checkBoxBypass.clicked.connect(self.bypassModeChecked)
        self.checkBoxMute.clicked.connect(self.muteModeChecked)
        self.checkBoxEnabled.clicked.connect(self.webEnabledChecked)
        self.checkBoxPaired.clicked.connect(self.remotePairedChecked)

    def pedalModeChecked(self):
        self.info["pedalMode"] = self.checkBoxPedalMode.isChecked()
        self.updateInfoCallback(self.info)

    def bypassModeChecked(self):
        self.info["bypassMode"] = self.checkBoxBypass.isChecked()
        self.updateInfoCallback(self.info)

    def muteModeChecked(self):
        self.info["muteMode"] = self.checkBoxMute.isChecked()
        self.updateInfoCallback(self.info)

    def webEnabledChecked(self):
        self.info["webEnabled"] = self.checkBoxEnabled.isChecked()
        self.updateInfoCallback(self.info)

    def remotePairedChecked(self):
        self.info["remotePaired"] = self.checkBoxPaired.isChecked()
        self.updateInfoCallback(self.info)

    def updateInfo(self, info):
        self.info = info
        self.lcdNumberBank.display(info["bank"])
        self.lcdNumberPreset.display(info["preset"])
        self.lineEditPresetName.setText(info["presetName"])
        self.lineEditWebAddress.setText(info["webAddress"])
        self.lineEditRemoteID.setText(info["remoteID"])
        self.checkBoxPedalMode.setChecked(info["pedalMode"])
        self.checkBoxBypass.setChecked(info["bypassMode"])
        self.checkBoxMute.setChecked(info["muteMode"])
        self.checkBoxEnabled.setChecked(info["webEnabled"])
        self.checkBoxPaired.setChecked(info["remotePaired"])

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = LCDWindow()
    window.show()
    sys.exit(app.exec_())
