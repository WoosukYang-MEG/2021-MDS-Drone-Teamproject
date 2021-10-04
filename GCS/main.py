import sys
import serial.tools.list_ports

from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUiType

form_class1 = loadUiType("pytq5_GCS.ui")[0]


class Main_GCS(QMainWindow, form_class1):
    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)
        self.setupUi(self)

        self.button_Connect2Drone.clicked.connect(self.click_on_button_Connect2Drone)
        self.button_Connect2GUI.clicked.connect(self.click_on_button_Connect2GUI)
        self.button_SerialPortRefresh.clicked.connect(self.click_on_button_SerialPortRefresh)

        self.click_on_button_SerialPortRefresh()

        self.thread3_send_ref_value = SendRefVal()

    def __del__(self):
        print(".... end Main_GCS thread.....")
        self.wait()

    def click_on_button_Connect2Drone(self):
        port_Cmd = self.combobox_Drone_serialPort.currentText()
        print(port_Cmd)
        serialPort2Drone = serial.Serial(port=port_Cmd,
                                         baudrate=9600,
                                         parity=serial.PARITY_NONE,
                                         stopbits=serial.STOPBITS_ONE,
                                         bytesize=serial.EIGHTBITS,
                                         timeout=0
                                         )
        self.thread1_drone_to_GUI = DroneToGUI(serialPort2Drone, parent=self)

    def click_on_button_Connect2GUI(self):
        port_GUI = self.combobox_GUI_serialPort.currentText()
        print(port_GUI)
        serialPort2GUI = serial.Serial(port=port_GUI,
                                       baudrate=9600,
                                       parity=serial.PARITY_NONE,
                                       stopbits=serial.STOPBITS_ONE,
                                       bytesize=serial.EIGHTBITS,
                                       timeout=0
                                       )
        self.thread2_GUI_to_drone = GUIToDrone(serialPort2GUI, parent=self)

    def click_on_button_SerialPortRefresh(self):
        self.combobox_Drone_serialPort.clear()
        self.combobox_GUI_serialPort.clear()

        ports = serial.tools.list_ports.comports()
        for port in ports:
            self.combobox_Drone_serialPort.addItem(port.name)
            self.combobox_GUI_serialPort.addItem(port.name)

    def click_send_ref_value(self):
        self.thread3_send_ref_value.run()


class DroneToGUI(QThread):  # Thread1 : port9 -> cmd -> port3
    def __init__(self, serial_input, parent=None):
        super().__init__()
        self.main_GCS = parent
        self.recv_buffer = bytes(20)  # 0x00/0x00 ... /0x00
        self.serial_input = serial_input  # 이렇게 하면 main 에 있는 thread에 바로 접근이 되는건지? 아니면 복사해 와서 접근 못하는지
        # self.ser_write = self.main_GCS.ser_GUI_to_drone

    def run(self):
        while True:
            self.main_GCS.mutex.lock()
            if self.serial_input.readble():
                self.recv_buffer = self.serial_input.read(20)  # recieve 20 bytes
                print(f"{str(self.recv_buffer, 'utf-8')}")
                self.main_GCS.textBrowser.setPlainText(str(self.recv_buffer, 'utf-8'))
                # self.ser_write(self.recv_buffer)  # send 20 bytes
            self.main_GCS.mutex.unlock()

    def __del__(self):
        self.serial_input.close()
        print(".... end DroneToGUI thread.....")
        self.wait()


class GUIToDrone(QThread):  # Thread2 : port3 -> cmd -> port9
    def __init__(self, serial_input, parent=None):
        super().__init__()
        self.main_GCS = parent
        self.recv_buffer = bytes(20)
        self.serial_input = serial_input
        # self.ser_wirte = self.main_GCS.ser_drone_to_GUI

    def run(self):
        while True:
            self.main_GCS.mutex.lock()
            if self.serial_input.readble():
                self.recv_buffer = self.serial_input.read(20)  # recieve 20 bytes
                print(f"{str(self.recv_buffer, 'utf-8')}")
                self.main_GCS.textBrowser.setPlainText(str(self.recv_buffer, 'utf-8'))
                # self.ser_wirte.write(self.recv_buffer)  # send 20 bytes
            self.main_GCS.mutex.unlock()

    def __del__(self):
        self.serial_input.close()
        print(".... end GUIToDrone thread.....")
        self.wait()


class SendRefVal(QThread):  # Thread2 : port3 -> cmd -> port9
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent
        self.ref_buffer = self.main_GCS.ref_buffer
        self.ser_wirte = self.main_GCS.ser_drone_to_GUI

    def run(self):
        self.main_GCS.mutex.lock()
        # TODO
        self.ser_wirte.write(self.ref_buffer)  # send 20 bytes

        self.main_GCS.mutex.unlock()


# class MapWidget(QThread):
#     def __init__(self, parent=None):
#         super().__init__()
#         self.main_GCS = parent
#
#         i = 0
#         data = io.BytesIO()
#         coordinate = (37.5556463 + i, 126.9280658 + i)
#         m = folium.Map(zoom_start=18, location=coordinate)
#         marker = folium.Marker(coordinate, popup="홍대 입구", icon=folium.Icon(icon='red'))
#
#         marker.add_to(m)
#
#         m.save(data, close_file=False)
#         self.main_GCS.webEngineView.setHtml(data.getvalue().decode())
#
#     def __del__(self):
#         print(".... end map thread.....")
#         self.wait()
#
#
# class CameraWidget(QThread):
#     def __init__(self, parent=None):
#         super().__init__()
#         self.main_GCS = parent
#         self.is_camera_activated = True
#
#         self.cap = cv2.VideoCapture("test_video2.avi")
#
#     def run(self):
#         while self.cap.isOpened():
#             ret, img = self.cap.read()
#             if ret:
#                 img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
#                 h, w, c = img.shape
#                 qImg = QtGui.QImage(img.data, w, h, w * c, QtGui.QImage.Format_RGB888)
#                 pixmap = QtGui.QPixmap.fromImage(qImg)
#                 self.main_GCS.label.setPixmap(pixmap)
#                 cv2.waitKey(1)
#             else:
#                 QtWidgets.QMessageBox.about(self.main, "Error", "Cannot read frame.")
#                 print("cannot read frame.")
#                 break
#         self.cap.release()
#
#     def __del__(self):
#         print(".... end camera thread.....")
#         self.wait()
#
#
# class ComPortWidget(QThread):  # TODO
#     def __init__(self, parent=None):
#         super().__init__()
#         self.main_GCS = parent


if __name__ == "__main__":
    app = QApplication(sys.argv)
    myWindow = Main_GCS(None)
    myWindow.show()

    try:
        sys.exit(app.exec_())
    except SystemExit:
        print('Closing Window...')
