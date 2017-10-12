# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'lcdWindow.ui'
#
# Created: Thu Oct  5 13:55:19 2017
#      by: PyQt4 UI code generator 4.11.3
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    def _fromUtf8(s):
        return s

try:
    _encoding = QtGui.QApplication.UnicodeUTF8
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig, _encoding)
except AttributeError:
    def _translate(context, text, disambig):
        return QtGui.QApplication.translate(context, text, disambig)

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName(_fromUtf8("MainWindow"))
        MainWindow.resize(480, 272)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName(_fromUtf8("centralwidget"))
        self.gridLayoutMain = QtGui.QGridLayout(self.centralwidget)
        self.gridLayoutMain.setObjectName(_fromUtf8("gridLayoutMain"))
        self.groupBoxInfo = QtGui.QGroupBox(self.centralwidget)
        self.groupBoxInfo.setObjectName(_fromUtf8("groupBoxInfo"))
        self.gridLayoutInfo = QtGui.QGridLayout(self.groupBoxInfo)
        self.gridLayoutInfo.setObjectName(_fromUtf8("gridLayoutInfo"))
        self.labelBank = QtGui.QLabel(self.groupBoxInfo)
        self.labelBank.setObjectName(_fromUtf8("labelBank"))
        self.gridLayoutInfo.addWidget(self.labelBank, 1, 0, 1, 1)
        self.checkBoxPedalMode = QtGui.QCheckBox(self.groupBoxInfo)
        self.checkBoxPedalMode.setCheckable(True)
        self.checkBoxPedalMode.setObjectName(_fromUtf8("checkBoxPedalMode"))
        self.gridLayoutInfo.addWidget(self.checkBoxPedalMode, 1, 3, 1, 1)
        self.checkBoxBypass = QtGui.QCheckBox(self.groupBoxInfo)
        self.checkBoxBypass.setCheckable(True)
        self.checkBoxBypass.setObjectName(_fromUtf8("checkBoxBypass"))
        self.gridLayoutInfo.addWidget(self.checkBoxBypass, 2, 3, 1, 1)
        self.checkBoxMute = QtGui.QCheckBox(self.groupBoxInfo)
        self.checkBoxMute.setCheckable(True)
        self.checkBoxMute.setObjectName(_fromUtf8("checkBoxMute"))
        self.gridLayoutInfo.addWidget(self.checkBoxMute, 3, 3, 1, 1)
        self.labelPresetName = QtGui.QLabel(self.groupBoxInfo)
        self.labelPresetName.setObjectName(_fromUtf8("labelPresetName"))
        self.gridLayoutInfo.addWidget(self.labelPresetName, 3, 0, 1, 1)
        self.lcdNumberPreset = QtGui.QLCDNumber(self.groupBoxInfo)
        self.lcdNumberPreset.setMinimumSize(QtCore.QSize(0, 30))
        self.lcdNumberPreset.setDigitCount(3)
        self.lcdNumberPreset.setSegmentStyle(QtGui.QLCDNumber.Flat)
        self.lcdNumberPreset.setObjectName(_fromUtf8("lcdNumberPreset"))
        self.gridLayoutInfo.addWidget(self.lcdNumberPreset, 2, 1, 1, 1)
        self.checkBoxEnabled = QtGui.QCheckBox(self.groupBoxInfo)
        self.checkBoxEnabled.setObjectName(_fromUtf8("checkBoxEnabled"))
        self.gridLayoutInfo.addWidget(self.checkBoxEnabled, 4, 3, 1, 1)
        self.labelPreset = QtGui.QLabel(self.groupBoxInfo)
        self.labelPreset.setObjectName(_fromUtf8("labelPreset"))
        self.gridLayoutInfo.addWidget(self.labelPreset, 2, 0, 1, 1)
        self.lineEditWebAddress = QtGui.QLineEdit(self.groupBoxInfo)
        self.lineEditWebAddress.setMinimumSize(QtCore.QSize(0, 30))
        self.lineEditWebAddress.setReadOnly(True)
        self.lineEditWebAddress.setObjectName(_fromUtf8("lineEditWebAddress"))
        self.gridLayoutInfo.addWidget(self.lineEditWebAddress, 4, 1, 1, 1)
        self.lineEditPresetName = QtGui.QLineEdit(self.groupBoxInfo)
        self.lineEditPresetName.setMinimumSize(QtCore.QSize(0, 30))
        self.lineEditPresetName.setReadOnly(True)
        self.lineEditPresetName.setObjectName(_fromUtf8("lineEditPresetName"))
        self.gridLayoutInfo.addWidget(self.lineEditPresetName, 3, 1, 1, 1)
        self.labelWebAddress = QtGui.QLabel(self.groupBoxInfo)
        self.labelWebAddress.setObjectName(_fromUtf8("labelWebAddress"))
        self.gridLayoutInfo.addWidget(self.labelWebAddress, 4, 0, 1, 1)
        self.lcdNumberBank = QtGui.QLCDNumber(self.groupBoxInfo)
        self.lcdNumberBank.setMinimumSize(QtCore.QSize(0, 30))
        self.lcdNumberBank.setDigitCount(3)
        self.lcdNumberBank.setSegmentStyle(QtGui.QLCDNumber.Flat)
        self.lcdNumberBank.setObjectName(_fromUtf8("lcdNumberBank"))
        self.gridLayoutInfo.addWidget(self.lcdNumberBank, 1, 1, 1, 1)
        self.labelRemoteID = QtGui.QLabel(self.groupBoxInfo)
        self.labelRemoteID.setObjectName(_fromUtf8("labelRemoteID"))
        self.gridLayoutInfo.addWidget(self.labelRemoteID, 5, 0, 1, 1)
        self.lineEditRemoteID = QtGui.QLineEdit(self.groupBoxInfo)
        self.lineEditRemoteID.setMinimumSize(QtCore.QSize(0, 30))
        self.lineEditRemoteID.setReadOnly(True)
        self.lineEditRemoteID.setObjectName(_fromUtf8("lineEditRemoteID"))
        self.gridLayoutInfo.addWidget(self.lineEditRemoteID, 5, 1, 1, 1)
        self.checkBoxPaired = QtGui.QCheckBox(self.groupBoxInfo)
        self.checkBoxPaired.setEnabled(False)
        self.checkBoxPaired.setCheckable(True)
        self.checkBoxPaired.setObjectName(_fromUtf8("checkBoxPaired"))
        self.gridLayoutInfo.addWidget(self.checkBoxPaired, 5, 3, 1, 1)
        spacerItem = QtGui.QSpacerItem(40, 20, QtGui.QSizePolicy.Expanding, QtGui.QSizePolicy.Minimum)
        self.gridLayoutInfo.addItem(spacerItem, 1, 2, 1, 1)
        self.gridLayoutMain.addWidget(self.groupBoxInfo, 0, 0, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(_translate("MainWindow", "Pedal Matrix", None))
        self.groupBoxInfo.setTitle(_translate("MainWindow", "Info", None))
        self.labelBank.setText(_translate("MainWindow", "Bank:", None))
        self.checkBoxPedalMode.setText(_translate("MainWindow", "Pedal Mode", None))
        self.checkBoxBypass.setText(_translate("MainWindow", "Bypass", None))
        self.checkBoxMute.setText(_translate("MainWindow", "Mute", None))
        self.labelPresetName.setText(_translate("MainWindow", "Preset Name:", None))
        self.checkBoxEnabled.setText(_translate("MainWindow", "Enabled", None))
        self.labelPreset.setText(_translate("MainWindow", "Preset:", None))
        self.labelWebAddress.setText(_translate("MainWindow", "Web Address: ", None))
        self.labelRemoteID.setText(_translate("MainWindow", "Remote ID: ", None))
        self.checkBoxPaired.setText(_translate("MainWindow", "Paired", None))

