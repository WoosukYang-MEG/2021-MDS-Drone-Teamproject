import io
import sys

import cv2
import folium
from PyQt5 import QtGui
from PyQt5 import QtWidgets
from PyQt5.QtCore import *
from PyQt5.QtWidgets import *
from PyQt5.uic import loadUiType

form_class1 = loadUiType("pytq5_GCS.ui")[0]


class Main_GCS(QMainWindow, form_class1):
    def __init__(self, parent=None):
        QMainWindow.__init__(self, parent)
        self.setupUi(self)

        self.CameraOnButton.clicked.connect(self.click_on_button)
        self.CameraOffButton.clicked.connect(self.click_off_button)

        self.map_thread = MapWidget(parent=self)
        self.camera_thread = CameraWidget(parent=self)

    def click_on_button(self):
        self.camera_thread.run()

    def click_off_button(self):
        pass  # TODO


class MapWidget(QThread):
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent

        i = 0
        data = io.BytesIO()
        coordinate = (37.5556463 + i, 126.9280658 + i)
        m = folium.Map(zoom_start=18, location=coordinate)
        folium.Marker(coordinate, popup="홍대 입구", icon=folium.Icon(icon='red')).add_to(m)
        m.save(data, close_file=False)
        self.main_GCS.webEngineView.setHtml(data.getvalue().decode())

    def __del__(self):
        print(".... end map thread.....")
        self.wait()


class CameraWidget(QThread):
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent
        self.is_camera_activated = True

        self.cap = cv2.VideoCapture("test_video2.avi")

    def run(self):
        while self.cap.isOpened():
            ret, img = self.cap.read()
            if ret:
                img = cv2.cvtColor(img, cv2.COLOR_BGR2RGB)
                h, w, c = img.shape
                qImg = QtGui.QImage(img.data, w, h, w * c, QtGui.QImage.Format_RGB888)
                pixmap = QtGui.QPixmap.fromImage(qImg)
                self.main_GCS.label.setPixmap(pixmap)
                cv2.waitKey(1)
            else:
                QtWidgets.QMessageBox.about(self.main, "Error", "Cannot read frame.")
                print("cannot read frame.")
                break
        self.cap.release()

    def __del__(self):
        print(".... end camera thread.....")
        self.wait()


class ComPortWidget(QThread):  # TODO
    def __init__(self, parent=None):
        super().__init__()
        self.main_GCS = parent


if __name__ == "__main__":
    app = QApplication(sys.argv)
    myWindow = Main_GCS(None)
    myWindow.show()

    try:
        sys.exit(app.exec_())
    except SystemExit:
        print('Closing Window...')
