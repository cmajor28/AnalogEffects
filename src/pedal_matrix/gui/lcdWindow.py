# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'lcdWindow.ui'
#
# Created: Thu Mar 15 03:30:16 2018
#      by: pyside-uic 0.2.15 running on PySide 1.2.2
#
# WARNING! All changes made in this file will be lost!

from PySide import QtCore, QtGui

class Ui_MainWindow(object):
    def setupUi(self, MainWindow):
        MainWindow.setObjectName("MainWindow")
        MainWindow.resize(525, 306)
        self.centralwidget = QtGui.QWidget(MainWindow)
        self.centralwidget.setObjectName("centralwidget")
        self.gridLayoutMain = QtGui.QGridLayout(self.centralwidget)
        self.gridLayoutMain.setObjectName("gridLayoutMain")
        self.tabWidget = QtGui.QTabWidget(self.centralwidget)
        self.tabWidget.setEnabled(True)
        self.tabWidget.setStyleSheet("font: 12pt \"MS Shell Dlg 2\";")
        self.tabWidget.setObjectName("tabWidget")
        self.tabSystemInfo = QtGui.QWidget()
        self.tabSystemInfo.setObjectName("tabSystemInfo")
        self.gridLayout_2 = QtGui.QGridLayout(self.tabSystemInfo)
        self.gridLayout_2.setObjectName("gridLayout_2")
        self.lineEditRemoteID = QtGui.QLineEdit(self.tabSystemInfo)
        self.lineEditRemoteID.setReadOnly(True)
        self.lineEditRemoteID.setObjectName("lineEditRemoteID")
        self.gridLayout_2.addWidget(self.lineEditRemoteID, 4, 1, 1, 1)
        self.labelHardwareVersion = QtGui.QLabel(self.tabSystemInfo)
        self.labelHardwareVersion.setObjectName("labelHardwareVersion")
        self.gridLayout_2.addWidget(self.labelHardwareVersion, 0, 0, 1, 1)
        self.lineEditHardwareVersion = QtGui.QLineEdit(self.tabSystemInfo)
        self.lineEditHardwareVersion.setReadOnly(True)
        self.lineEditHardwareVersion.setObjectName("lineEditHardwareVersion")
        self.gridLayout_2.addWidget(self.lineEditHardwareVersion, 0, 1, 1, 1)
        self.labelWebAddress = QtGui.QLabel(self.tabSystemInfo)
        self.labelWebAddress.setObjectName("labelWebAddress")
        self.gridLayout_2.addWidget(self.labelWebAddress, 3, 0, 1, 1)
        self.checkBoxPaired = QtGui.QCheckBox(self.tabSystemInfo)
        self.checkBoxPaired.setEnabled(False)
        self.checkBoxPaired.setCheckable(True)
        self.checkBoxPaired.setObjectName("checkBoxPaired")
        self.gridLayout_2.addWidget(self.checkBoxPaired, 4, 2, 1, 1)
        self.labelSoftwareVersion = QtGui.QLabel(self.tabSystemInfo)
        self.labelSoftwareVersion.setObjectName("labelSoftwareVersion")
        self.gridLayout_2.addWidget(self.labelSoftwareVersion, 1, 0, 1, 1)
        self.labelWebsite = QtGui.QLabel(self.tabSystemInfo)
        self.labelWebsite.setObjectName("labelWebsite")
        self.gridLayout_2.addWidget(self.labelWebsite, 2, 0, 1, 1)
        self.checkBoxEnabled = QtGui.QCheckBox(self.tabSystemInfo)
        self.checkBoxEnabled.setObjectName("checkBoxEnabled")
        self.gridLayout_2.addWidget(self.checkBoxEnabled, 3, 2, 1, 1)
        self.labelRemoteID = QtGui.QLabel(self.tabSystemInfo)
        self.labelRemoteID.setObjectName("labelRemoteID")
        self.gridLayout_2.addWidget(self.labelRemoteID, 4, 0, 1, 1)
        self.lineEditSoftwareVersion = QtGui.QLineEdit(self.tabSystemInfo)
        self.lineEditSoftwareVersion.setReadOnly(True)
        self.lineEditSoftwareVersion.setObjectName("lineEditSoftwareVersion")
        self.gridLayout_2.addWidget(self.lineEditSoftwareVersion, 1, 1, 1, 1)
        self.lineEditWebAddress = QtGui.QLineEdit(self.tabSystemInfo)
        self.lineEditWebAddress.setReadOnly(True)
        self.lineEditWebAddress.setObjectName("lineEditWebAddress")
        self.gridLayout_2.addWidget(self.lineEditWebAddress, 3, 1, 1, 1)
        self.lineEditWebsite = QtGui.QLineEdit(self.tabSystemInfo)
        self.lineEditWebsite.setReadOnly(True)
        self.lineEditWebsite.setObjectName("lineEditWebsite")
        self.gridLayout_2.addWidget(self.lineEditWebsite, 2, 1, 1, 1)
        self.tabWidget.addTab(self.tabSystemInfo, "")
        self.tabPedalMatrix = QtGui.QWidget()
        self.tabPedalMatrix.setObjectName("tabPedalMatrix")
        self.gridLayout = QtGui.QGridLayout(self.tabPedalMatrix)
        self.gridLayout.setObjectName("gridLayout")
        self.checkBoxPedalMode = QtGui.QCheckBox(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.checkBoxPedalMode.setFont(font)
        self.checkBoxPedalMode.setCheckable(True)
        self.checkBoxPedalMode.setObjectName("checkBoxPedalMode")
        self.gridLayout.addWidget(self.checkBoxPedalMode, 0, 2, 1, 1)
        self.labelPresetName = QtGui.QLabel(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.labelPresetName.setFont(font)
        self.labelPresetName.setObjectName("labelPresetName")
        self.gridLayout.addWidget(self.labelPresetName, 3, 0, 1, 1)
        self.lcdNumberBank = QtGui.QLCDNumber(self.tabPedalMatrix)
        self.lcdNumberBank.setMinimumSize(QtCore.QSize(120, 33))
        self.lcdNumberBank.setMaximumSize(QtCore.QSize(16777215, 16777215))
        self.lcdNumberBank.setDigitCount(3)
        self.lcdNumberBank.setSegmentStyle(QtGui.QLCDNumber.Flat)
        self.lcdNumberBank.setObjectName("lcdNumberBank")
        self.gridLayout.addWidget(self.lcdNumberBank, 0, 1, 1, 1)
        self.lineEditPresetName = QtGui.QLineEdit(self.tabPedalMatrix)
        self.lineEditPresetName.setMinimumSize(QtCore.QSize(120, 25))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.lineEditPresetName.setFont(font)
        self.lineEditPresetName.setReadOnly(True)
        self.lineEditPresetName.setObjectName("lineEditPresetName")
        self.gridLayout.addWidget(self.lineEditPresetName, 3, 1, 1, 1)
        self.labelBank = QtGui.QLabel(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.labelBank.setFont(font)
        self.labelBank.setObjectName("labelBank")
        self.gridLayout.addWidget(self.labelBank, 0, 0, 1, 1)
        self.lineEditBankName = QtGui.QLineEdit(self.tabPedalMatrix)
        self.lineEditBankName.setMinimumSize(QtCore.QSize(120, 25))
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.lineEditBankName.setFont(font)
        self.lineEditBankName.setText("")
        self.lineEditBankName.setReadOnly(True)
        self.lineEditBankName.setObjectName("lineEditBankName")
        self.gridLayout.addWidget(self.lineEditBankName, 1, 1, 1, 1)
        self.lcdNumberPreset = QtGui.QLCDNumber(self.tabPedalMatrix)
        self.lcdNumberPreset.setMinimumSize(QtCore.QSize(120, 33))
        self.lcdNumberPreset.setDigitCount(3)
        self.lcdNumberPreset.setSegmentStyle(QtGui.QLCDNumber.Flat)
        self.lcdNumberPreset.setObjectName("lcdNumberPreset")
        self.gridLayout.addWidget(self.lcdNumberPreset, 2, 1, 1, 1)
        self.labelPreset = QtGui.QLabel(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.labelPreset.setFont(font)
        self.labelPreset.setObjectName("labelPreset")
        self.gridLayout.addWidget(self.labelPreset, 2, 0, 1, 1)
        self.checkBoxBypass = QtGui.QCheckBox(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.checkBoxBypass.setFont(font)
        self.checkBoxBypass.setCheckable(True)
        self.checkBoxBypass.setObjectName("checkBoxBypass")
        self.gridLayout.addWidget(self.checkBoxBypass, 1, 2, 1, 1)
        self.checkBoxMute = QtGui.QCheckBox(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.checkBoxMute.setFont(font)
        self.checkBoxMute.setCheckable(True)
        self.checkBoxMute.setObjectName("checkBoxMute")
        self.gridLayout.addWidget(self.checkBoxMute, 2, 2, 1, 1)
        self.labelBankName = QtGui.QLabel(self.tabPedalMatrix)
        font = QtGui.QFont()
        font.setFamily("MS Shell Dlg 2")
        font.setPointSize(12)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.labelBankName.setFont(font)
        self.labelBankName.setObjectName("labelBankName")
        self.gridLayout.addWidget(self.labelBankName, 1, 0, 1, 1)
        self.tabWidget.addTab(self.tabPedalMatrix, "")
        self.tabPedalArrangement = QtGui.QWidget()
        self.tabPedalArrangement.setStyleSheet("font: 16pt \"MS Shell Dlg 2\";")
        self.tabPedalArrangement.setObjectName("tabPedalArrangement")
        self.horizontalLayout = QtGui.QHBoxLayout(self.tabPedalArrangement)
        self.horizontalLayout.setObjectName("horizontalLayout")
        self.labelIn = QtGui.QLabel(self.tabPedalArrangement)
        self.labelIn.setMinimumSize(QtCore.QSize(70, 90))
        self.labelIn.setMaximumSize(QtCore.QSize(70, 90))
        self.labelIn.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelIn.setAlignment(QtCore.Qt.AlignCenter)
        self.labelIn.setObjectName("labelIn")
        self.horizontalLayout.addWidget(self.labelIn)
        self.gridLayoutPedals = QtGui.QGridLayout()
        self.gridLayoutPedals.setObjectName("gridLayoutPedals")
        self.labelPedal3 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal3.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal3.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal3.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal3.setText("")
        self.labelPedal3.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal3.setObjectName("labelPedal3")
        self.gridLayoutPedals.addWidget(self.labelPedal3, 0, 3, 1, 1)
        self.labelPedal1 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal1.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal1.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal1.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal1.setText("")
        self.labelPedal1.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal1.setObjectName("labelPedal1")
        self.gridLayoutPedals.addWidget(self.labelPedal1, 0, 0, 1, 1)
        self.labelPedal7 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal7.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal7.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal7.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal7.setText("")
        self.labelPedal7.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal7.setObjectName("labelPedal7")
        self.gridLayoutPedals.addWidget(self.labelPedal7, 1, 3, 1, 1)
        self.labelPedal4 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal4.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal4.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal4.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal4.setText("")
        self.labelPedal4.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal4.setObjectName("labelPedal4")
        self.gridLayoutPedals.addWidget(self.labelPedal4, 0, 4, 1, 1)
        self.labelPedal5 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal5.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal5.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal5.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal5.setText("")
        self.labelPedal5.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal5.setObjectName("labelPedal5")
        self.gridLayoutPedals.addWidget(self.labelPedal5, 1, 0, 1, 1)
        self.labelPedal2 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal2.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal2.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal2.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal2.setText("")
        self.labelPedal2.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal2.setObjectName("labelPedal2")
        self.gridLayoutPedals.addWidget(self.labelPedal2, 0, 2, 1, 1)
        self.labelPedal6 = QtGui.QLabel(self.tabPedalArrangement)
        self.labelPedal6.setMinimumSize(QtCore.QSize(70, 90))
        self.labelPedal6.setMaximumSize(QtCore.QSize(70, 90))
        self.labelPedal6.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelPedal6.setText("")
        self.labelPedal6.setAlignment(QtCore.Qt.AlignCenter)
        self.labelPedal6.setObjectName("labelPedal6")
        self.gridLayoutPedals.addWidget(self.labelPedal6, 1, 2, 1, 1)
        self.horizontalLayoutAmpControl = QtGui.QHBoxLayout()
        self.horizontalLayoutAmpControl.setObjectName("horizontalLayoutAmpControl")
        self.labelTip = QtGui.QLabel(self.tabPedalArrangement)
        font = QtGui.QFont()
        font.setFamily("Consolas")
        font.setPointSize(16)
        font.setWeight(50)
        font.setItalic(False)
        font.setBold(False)
        self.labelTip.setFont(font)
        self.labelTip.setStyleSheet("font: 16pt \"Consolas\";")
        self.labelTip.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelTip.setAlignment(QtCore.Qt.AlignCenter)
        self.labelTip.setObjectName("labelTip")
        self.horizontalLayoutAmpControl.addWidget(self.labelTip)
        self.labelRing = QtGui.QLabel(self.tabPedalArrangement)
        self.labelRing.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelRing.setAlignment(QtCore.Qt.AlignCenter)
        self.labelRing.setObjectName("labelRing")
        self.horizontalLayoutAmpControl.addWidget(self.labelRing)
        self.gridLayoutPedals.addLayout(self.horizontalLayoutAmpControl, 1, 4, 1, 1)
        self.horizontalLayout.addLayout(self.gridLayoutPedals)
        self.labelOut = QtGui.QLabel(self.tabPedalArrangement)
        self.labelOut.setEnabled(True)
        self.labelOut.setMinimumSize(QtCore.QSize(70, 90))
        self.labelOut.setMaximumSize(QtCore.QSize(70, 90))
        self.labelOut.setFrameShape(QtGui.QFrame.WinPanel)
        self.labelOut.setAlignment(QtCore.Qt.AlignCenter)
        self.labelOut.setObjectName("labelOut")
        self.horizontalLayout.addWidget(self.labelOut)
        self.tabWidget.addTab(self.tabPedalArrangement, "")
        self.gridLayoutMain.addWidget(self.tabWidget, 0, 1, 1, 1)
        MainWindow.setCentralWidget(self.centralwidget)

        self.retranslateUi(MainWindow)
        self.tabWidget.setCurrentIndex(1)
        QtCore.QMetaObject.connectSlotsByName(MainWindow)

    def retranslateUi(self, MainWindow):
        MainWindow.setWindowTitle(QtGui.QApplication.translate("MainWindow", "Pedal Matrix", None, QtGui.QApplication.UnicodeUTF8))
        self.labelHardwareVersion.setText(QtGui.QApplication.translate("MainWindow", "Hardware Version:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelWebAddress.setText(QtGui.QApplication.translate("MainWindow", "Web Address:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxPaired.setText(QtGui.QApplication.translate("MainWindow", "Paired", None, QtGui.QApplication.UnicodeUTF8))
        self.labelSoftwareVersion.setText(QtGui.QApplication.translate("MainWindow", "Software Version:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelWebsite.setText(QtGui.QApplication.translate("MainWindow", "Website:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxEnabled.setText(QtGui.QApplication.translate("MainWindow", "Enabled", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRemoteID.setText(QtGui.QApplication.translate("MainWindow", "Remote ID:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabSystemInfo), QtGui.QApplication.translate("MainWindow", "System Info", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxPedalMode.setText(QtGui.QApplication.translate("MainWindow", "Pedal Mode", None, QtGui.QApplication.UnicodeUTF8))
        self.labelPresetName.setText(QtGui.QApplication.translate("MainWindow", "Preset Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBank.setText(QtGui.QApplication.translate("MainWindow", "Bank:", None, QtGui.QApplication.UnicodeUTF8))
        self.labelPreset.setText(QtGui.QApplication.translate("MainWindow", "Preset:", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxBypass.setText(QtGui.QApplication.translate("MainWindow", "Bypass", None, QtGui.QApplication.UnicodeUTF8))
        self.checkBoxMute.setText(QtGui.QApplication.translate("MainWindow", "Mute", None, QtGui.QApplication.UnicodeUTF8))
        self.labelBankName.setText(QtGui.QApplication.translate("MainWindow", "Bank Name:", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabPedalMatrix), QtGui.QApplication.translate("MainWindow", "Pedal Matrix", None, QtGui.QApplication.UnicodeUTF8))
        self.labelIn.setText(QtGui.QApplication.translate("MainWindow", "IN", None, QtGui.QApplication.UnicodeUTF8))
        self.labelTip.setText(QtGui.QApplication.translate("MainWindow", "T", None, QtGui.QApplication.UnicodeUTF8))
        self.labelRing.setText(QtGui.QApplication.translate("MainWindow", "R", None, QtGui.QApplication.UnicodeUTF8))
        self.labelOut.setText(QtGui.QApplication.translate("MainWindow", "OUT", None, QtGui.QApplication.UnicodeUTF8))
        self.tabWidget.setTabText(self.tabWidget.indexOf(self.tabPedalArrangement), QtGui.QApplication.translate("MainWindow", "Pedal Arrangement", None, QtGui.QApplication.UnicodeUTF8))

