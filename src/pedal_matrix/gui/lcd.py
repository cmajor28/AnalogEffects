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
        QMetaObject.invokeMethod(self, "handler", Qt.QueuedConnection)

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

    def __updatePedals(self, pedals, enabled, presence, control, mute, bypass):
        for i in range(0,7):
            if pedals[i] > 0:
                numStr = str(pedals[i])
            else:
                numStr = ""
            eval("self.labelPedal{0}.setText(numStr)".format(i+1))
            eval("self.labelPedal{0}.setEnabled(enabled[pedals[i]-1])".format(i+1))

            if pedals[i] > 0:
                if presence[pedals[i]-1]:
                    # Pedal connected
                    eval("self.labelPedal{0}.setStyleSheet('color: blue')".format(i+1))
                else:
                    # Pedal disconnected
                    eval("self.labelPedal{0}.setStyleSheet('color: red')".format(i+1))
            else:
                # Pedal not used
                eval("self.labelPedal{0}.setStyleSheet('color: black')".format(i+1))

        self.labelIn.setEnabled(not bypass)
        self.labelOut.setEnabled(not mute)

        if presence[7]:
            self.labelIn.setStyleSheet('color: black')
        else:
            self.labelIn.setStyleSheet('color: red')

        if presence[8]:
            self.labelOut.setStyleSheet('color: black')
        else:
            self.labelOut.setStyleSheet('color: red')

        if control[1]:
            self.labelTip.setStyleSheet('color: blue')
        else:
            self.labelTip.setStyleSheet('color: black')

        if control[0]:
            self.labelRing.setStyleSheet('color: blue')
        else:
            self.labelRing.setStyleSheet('color: black')

    def updateInfo(self, info):
        self.info = info
        self.lineEditHardwareVersion.setText(info["hardwareVersion"])
        self.lineEditSoftwareVersion.setText(info["softwareVersion"])
        self.lineEditWebsite.setText(info["website"])
        self.lcdNumberBank.display(str(info["bank"]))
        self.lineEditBankName.setText(info["bankName"])
        self.lcdNumberPreset.display(str(info["preset"]))
        self.lineEditPresetName.setText(info["presetName"])
        self.lineEditWebAddress.setText(info["webAddress"])
        self.lineEditRemoteID.setText(info["remoteID"])
        self.checkBoxPedalMode.setChecked(info["pedalMode"])
        self.checkBoxBypass.setChecked(info["bypassMode"])
        self.checkBoxMute.setChecked(info["muteMode"])
        self.checkBoxEnabled.setChecked(info["webEnabled"])
        self.checkBoxPaired.setChecked(info["remotePaired"])
        self.__updatePedals(info["pedals"], info["enabled"], info["presence"], info["control"], info["muteMode"], info["bypassMode"])

if __name__ == '__main__':
    app = QApplication(sys.argv)
    window = LCDWindow()
    window.show()
    sys.exit(app.exec_())
