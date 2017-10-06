import sys
from PySide.QtCore import *
from PySide.QtGui import *
from lcdWindow import Ui_MainWindow

class LCDWindow(QMainWindow, Ui_MainWindow):

    def __init__(self, info, updateInfoCallback):
        super(LCDWindow, self).__init__()
        self.setupUi(self)
        self.showMaximized()
        self.setWindowFlags(Qt.FramelessWindowHint)

        self.info = info
        self.updateInfoCallback = updateInfoCallback
        self.checkBoxPedalMode.clicked.connect(self.pedalModeChecked)
        self.checkBoxBypass.clicked.connect(self.bypassModeChecked)
        self.checkBoxMute.clicked.connect(self.muteModeeChecked)
        self.checkBoxEnabled.clicked.connect(self.webEnabledChecked)
        self.checkBoxPaired.clicked.connect(self.remotePairedChecked)

    def pedalModeChecked(self, checked):
        self.info["pedalMode"] = checked
        self.updateInfoCallback(self.info)

    def bypassModeChecked(self, checked):
        self.info["bypassMode"] = checked
        self.updateInfoCallback(self.info)

    def muteModeChecked(self, checked):
        self.info["muteMode"] = checked
        self.updateInfoCallback(self.info)

    def webEnabledChecked(self, checked):
        self.info["webEnabled"] = checked
        self.updateInfoCallback(self.info)

    def remotePairedChecked(self, checked):
        self.info["remotePaired"] = checked
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
