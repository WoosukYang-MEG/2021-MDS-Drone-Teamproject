import sys
import struct

import serial.tools.list_ports
import cv2

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUiType
from PyQt5 import QtGui
from PyQt5 import QtWidgets

form_class1 = loadUiType("pytq5_GCS.ui")[0]


class Main_GCS(QMainWindow, form_class1):
    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)
        self.setupUi(self)

        self.is_there_serialPort2Drone = 0
        self.is_there_serialPort2GUI = 0
        self.is_msg_from_drone_received = 0 #TODO

        self.mutex = QMutex()

        self.button_Connect2Drone.clicked.connect(self.click_on_button_Connect2Drone)
        self.button_Connect2GUI.clicked.connect(self.click_on_button_Connect2GUI)
        self.button_SerialPortRescan.clicked.connect(self.click_on_button_SerialPortRescan)
        self.button_CameraOn.clicked.connect(self.click_on_button_CameraOn)
        self.button_CameraOff.clicked.connect(self.click_on_button_CameraOff)
        self.button_SendCommand.clicked.connect(self.click_on_button_SendCommand)
        self.button_Drop.clicked.connect(self.click_on_button_Drop)


        self.click_on_button_SerialPortRescan()
        self.thread_camera = CameraWidget(parent=self)
        self.thread_Drone_to_GUI = DroneToGUI(parent=self)
        self.thread_GUI_to_Drone = GUIToDrone(parent=self)


    def click_on_button_CameraOn(self):
        self.thread_camera.start()

    def click_on_button_CameraOff(self):
        # TODO
        pass

    def click_on_button_Connect2Drone(self):
        port_Cmd = self.combobox_Drone_serialPort.currentText()
        self.serialPort2Drone = serial.Serial(port=port_Cmd,
                                         baudrate=115200,
                                         # parity=serial.PARITY_NONE,
                                         # stopbits=serial.STOPBITS_ONE,
                                         # bytesize=serial.EIGHTBITS,
                                         timeout=0
                                         )
        self.is_there_serialPort2Drone = 1
        self.thread_Drone_to_GUI.start()
        self.textBrowser_serialPort.append(port_Cmd + " is connected")


    def click_on_button_Connect2GUI(self):
        port_GUI = self.combobox_GUI_serialPort.currentText()
        self.serialPort2GUI = serial.Serial(port=port_GUI,
                                       baudrate=115200,
                                       # parity=serial.PARITY_NONE,
                                       # stopbits=serial.STOPBITS_ONE,
                                       # bytesize=serial.EIGHTBITS,
                                       timeout=0
                                       )
        self.is_there_serialPort2GUI = 1
        self.thread_GUI_to_Drone.start()
        self.textBrowser_serialPort.append(port_GUI + " is connected")

    def click_on_button_SerialPortRescan(self):
        self.combobox_Drone_serialPort.clear()
        self.combobox_GUI_serialPort.clear()

        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.combobox_Drone_serialPort.addItem(port.name)
            self.combobox_GUI_serialPort.addItem(port.name)

    # All data is reference value (with faill safe)
    # CMD -> FC  - SYNC  buffer[0:2] -> 0x47('G') 0x53('S')
    # ID - buffer[2]: 11,  GPS latitude <long 10e7> - buffer[3:7] ,  GPS longitude <long 10e7> - buffer[7:11]
    # buffer[16] : GCS fail safe, buffer[17:19] : height <cm - unsigned short>
    def click_on_button_SendCommand(self):
        lat = self.lineEdit_Latitude.text()     # Latitude
        long = self.lineEdit_Longitude.text()   # Longitude
        height = self.lineEdit_Height.text()    # Height

        command_buffer = bytearray(20)
        command_buffer[0] = 0x47
        command_buffer[1] = 0x53
        command_buffer[2] = 0x11

        bytearray_lat = bytearray(struct.pack("f", float(lat)))
        for i, bytearray_lat_value in enumerate(bytearray_lat):
            command_buffer[i + 3] = bytearray_lat_value

        bytearray_long = bytearray(struct.pack("f", float(long)))
        for i, bytearray_long_value in enumerate(bytearray_long):
            command_buffer[i + 7] = bytearray_long_value

        bytearray_height = bytearray(struct.pack("H", int(height)))
        for i, bytearray_height_value in enumerate(bytearray_height):
            command_buffer[i + 17] = bytearray_height_value

        check_sum = 0xff
        for command_buffer_value in command_buffer:
            check_sum -= command_buffer_value
            if check_sum < 0:
                check_sum += 0xff

        command_buffer[19] = check_sum
        print(bytes(command_buffer))
        if self.is_there_serialPort2Drone:
            # self.main_GCS.mutex.lock()
            self.serialPort2Drone.write(bytes(command_buffer))  # send 40 bytes
            self.textBrowser_command.append("Latitude : " + lat + ", Longitude : " + long + ", Height : " + height)

    def click_on_button_Drop(self):
        self.textBrowser_command.append("Drop The Beat")
        pass

class DroneToGUI(QThread):  # Thread1 : port9 -> cmd -> port3
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent

    def run(self):
        while self.main_GCS.is_there_serialPort2Drone:
            if self.main_GCS.serialPort2Drone.readable():
                self.main_GCS.mutex.lock()
                recv_buffer = self.main_GCS.serialPort2Drone.read(40)  # recieve 20 bytes
                # self.main_GCS.textBrowser.append(str(recv_buffer, 'utf-8'))
                if self.main_GCS.is_there_serialPort2GUI and len(recv_buffer) > 0:
                    print(recv_buffer)
                    self.main_GCS.serialPort2GUI.write(recv_buffer)  # send 20 bytes
                self.main_GCS.mutex.unlock()

    def __del__(self):
        self.serial_input.close()
        print(".... end DroneToGUI thread.....")
        self.wait()


class GUIToDrone(QThread):  # Thread2 : port3 -> cmd -> port9
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent

    def run(self):
        while self.main_GCS.is_there_serialPort2GUI:
            if self.main_GCS.serialPort2GUI.readable():
                self.main_GCS.mutex.lock()
                recv_buffer = self.main_GCS.serialPort2GUI.read(40)  # recieve 20 bytes
                # self.main_GCS.textBrowser.append(str(recv_buffer, 'utf-8'))

                if self.main_GCS.is_there_serialPort2Drone and len(recv_buffer) > 0:
                    self.main_GCS.serialPort2Drone.write(recv_buffer)  # send 20 bytes

                self.main_GCS.mutex.unlock()

    def __del__(self):
        self.serial_input.close()
        print(".... end GUIToDrone thread.....")
        self.wait()



class CameraWidget(QThread):
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent
        self.is_camera_activated = True

        self.cap = cv2.VideoCapture("test_video.mp4")

    def run(self):
        while self.cap.isOpened():
            ret, img = self.cap.read()
            if ret:
                img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
                h, w, c = img.shape
                qImg = QtGui.QImage(img.data, w, h, w * c, QtGui.QImage.Format_RGB888)
                pixmap = QtGui.QPixmap.fromImage(qImg)
                self.main_GCS.frame_FPV_camera.setPixmap(pixmap)
                cv2.waitKey(1)
            else:
                QtWidgets.QMessageBox.about(self.main, "Error", "Cannot read frame.")
                print("cannot read frame.")
                break
        self.cap.release()

    def __del__(self):
        print(".... end camera thread.....")
        self.wait()



if __name__ == "__main__":
    app = QApplication(sys.argv)
    myWindow = Main_GCS(None)
    myWindow.show()

    try:
        sys.exit(app.exec_())
    except SystemExit:
        print('Closing Window...')
