#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
import numpy as np
import cv2
import time
import signal
from skimage import io

import face_recognition

try:
    from PyQt5.QtGui import *
    from PyQt5.QtCore import *
    from PyQt5.QtWidgets import *
except ImportError:
    print("Please Use PyQt5!")

from libs.main import *
from libs import *


class VideoThread(QThread):
    change_pixmap_signal = pyqtSignal(np.ndarray)

    def __init__(self):
        super().__init__()
        self._run_flag = True
        self.stream_status = False
        self.webcam = None
        self.cap = None
        self.image_name = None
        self.save_image_flag = False
        self.captured = False
        self.out = None
        self.recording_state = "stop"
        
        #Face Recognition
        self.face_encodings = []
        self.face_names = []
        self.process_this_frame = True
        self.dataset_dir = 'dataset/'
        self.updateDataset()

    def updateWebcam(self,current_webcam):
        self.webcam = current_webcam
        print("Now Using: ",current_webcam)

    def updateDataset(self):
        image_filenames = filter(lambda x: x.endswith('.jpg'), os.listdir(self.dataset_dir))
        image_filenames = sorted(image_filenames)
        self.face_names = [x[:-len(image_filenames)] for x in image_filenames]

        full_paths_to_images = [self.dataset_dir + x for x in image_filenames]
        for path_to_image in full_paths_to_images:
            image = face_recognition.load_image_file(path_to_image)
            face_encoding = face_recognition.face_encodings(image)[0]
            self.face_encodings.append(face_encoding)

    def run(self):
        last_frame = None
        temp_face_encodings = []
        temp_face_locations = []
        while self._run_flag:

            if self.webcam is not None:
                last_webcam = self.webcam
                self.cap = cv2.VideoCapture(self.webcam)
                while (self.webcam == last_webcam) and self.stream_status:
                    
                    ret, cv_img = self.cap.read()

                    cv_img_resize = cv2.resize(cv_img, (0, 0), fx=0.25, fy=0.25)
                    rgb_cv_img = cv_img_resize[:, :, ::-1]

                    if self.process_this_frame:
                        temp_face_locations = face_recognition.face_locations(rgb_cv_img)
                        temp_face_encodings = face_recognition.face_encodings(rgb_cv_img, temp_face_locations)

                    temp_face_names = []
                    for face_encoding in temp_face_encodings:
                        matches = face_recognition.compare_faces(self.face_encodings, face_encoding)
                        name = "Unknown"

                        if (len(self.face_names) > 0):
                            face_distances = face_recognition.face_distance(self.face_encodings, face_encoding)
                            best_match_index = np.argmin(face_distances)
                            if matches[best_match_index]:
                                name = self.face_names[best_match_index]

                        temp_face_names.append(name)

                    self.process_this_frame = not self.process_this_frame
                     
                    for (top, right, bottom, left), name in zip(temp_face_locations, temp_face_names):
                        top *= 4
                        right *= 4
                        bottom *= 4
                        left *= 4

                        cv2.rectangle(cv_img, (left, top), (right, bottom), (0, 0, 255), 2)

                        cv2.rectangle(cv_img, (left, bottom - 35), (right, bottom), (0, 0, 255), cv2.FILLED)
                        font = cv2.FONT_HERSHEY_DUPLEX
                        cv2.putText(cv_img, name, (left + 6, bottom - 6), font, 1.0, (255, 255, 255), 1)


                    if self.out is not None and self.recording_state != "stop":
                       self.out.write(cv_img)

                    if self.out is not None and self.recording_state == "end":
                       self.out.release()
                       self.out = None
                       self.recording_state = "stop"
                       print("Video Saved!")

                    if not self.captured:
                        last_frame = cv_img

                    if ret:
                        self.change_pixmap_signal.emit(cv_img)

                    if self.save_image_flag:
                        self.save_image_flag = False
                        self.captured = False
                        if self.image_name is not None:
                            cv2.imwrite(self.image_name, last_frame)
                            print("Image Saved!")
                
                if last_frame is not None:
                    if self.save_image_flag:
                        self.captured = False
                        self.save_image_flag = False
                        if self.image_name is not None:
                            cv2.imwrite(self.image_name, last_frame)
                            print("Video Saved!")

                if self.out is not None:
                    self.out.release()
                    self.out = None
                    self.recording_state = "stop"
                    print("Video Saved!")

        if self.cap is not None:
            self.cap.release()

    def stop(self):
        """Sets run flag to False and waits for thread to finish"""
        self._run_flag = False
        self.wait()
        if self.cap is not None:
            self.cap.release()

    def updateStreamStatus(self, status):
        self.stream_status = status

    def updateImageName(self, name):
        self.image_name = name
        self.save_image_flag = True

    def CaptureImage(self):
        self.captured = True

    def updateVideoRecorder(self, vidrec, status):
        self.recording_state = status
        if self.recording_state == "initial":
            self.recording_state = "play"
            self.out = vidrec


class FaceRecognition(QMainWindow, Ui_MainWindow):
    def __init__(self):
        super().__init__()
        self.setupUi(self)
        self.setFixedSize(self.size())

        self.display_width = 640
        self.display_height = 480
        
        self.device_list = []

        self.timer = QTimer()
        self.timer.timeout.connect(self.update)
        self.timer.start(100)

        self.thread = VideoThread()

        self.thread.change_pixmap_signal.connect(self.update_image)

        self.webcam_list.currentTextChanged.connect(self.thread.updateWebcam)

        self.stream_status = False
        self.streamButton.pressed.connect(self.stream)
        self.captureImageButton.pressed.connect(self.captureImage)

        self.recording_status = "initial"
        self.recordingButton.pressed.connect(self.recordVideo)
        self.stopRecordingButton.pressed.connect(self.stopRecordVideo)

        self.allow_to_record_and_capture_flag = False

        self.thread.start()

    def stopRecordVideo(self):
        if self.allow_to_record_and_capture_flag:
            self.thread.updateVideoRecorder(None, "end")
            self.recording_status = "initial"
            self.recordingButton.setText("Start")

    def recordVideo(self):
        if self.allow_to_record_and_capture_flag and self.stream_status:
            if self.recording_status == "play":
                self.recording_status = "stop"
            elif self.recording_status == "stop":
                self.recording_status = "play"
        
            if self.recording_status == "initial":
                name = QFileDialog.getSaveFileName(self, 'Save File')
                if name[0] != "":
                    name,_ = os.path.splitext(str(name[0]))
                    name = str(name + ".avi") 
                    out = cv2.VideoWriter(name,cv2.VideoWriter_fourcc('M','J','P','G'), 10, (self.display_width, self.display_height))
                    self.thread.updateVideoRecorder(out, self.recording_status)
                    self.recording_status = "play"
            if self.recording_status == "play":
                self.recordingButton.setText("Pause")
            if self.recording_status == "stop":
                self.recordingButton.setText("Start")
    
    def captureImage(self):
        if self.allow_to_record_and_capture_flag:
            self.thread.CaptureImage()
            name = QFileDialog.getSaveFileName(self, 'Save File')
            if name[0] != "":
                name,_ = os.path.splitext(str(name[0]))
                name = str(name + ".jpg") 
                self.thread.updateImageName(name)

    def stream(self):
        self.allow_to_record_and_capture_flag = True
        self.stream_status = not self.stream_status
        self.thread.updateStreamStatus(self.stream_status)
        str_status = "Stop Stream" if self.stream_status else "Start Stream" 
        self.streamButton.setText(str_status)
        if not self.stream_status:
            self.recording_status = "initial"
            self.recordingButton.setText("Start")

    def update(self):
        for idx in range(0,64):
            device = str("/dev/video"+str(idx))
            cap = cv2.VideoCapture(device)
            ret, _ = cap.read()
            if ret:
                if device not in self.device_list:
                    self.device_list.append(device)
                    self.webcam_list.addItem(device)
            cap.release()

    def closeEvent(self, event):
        self.thread.updateStreamStatus(False)
        self.thread.stop()
        event.accept()

    @pyqtSlot(np.ndarray)
    def update_image(self, cv_img):
        """Updates the image_label with a new opencv image"""
        qt_img = self.convert_cv_qt(cv_img)
        self.streamed_video.setPixmap(qt_img)
    
    def convert_cv_qt(self, cv_img):
        """Convert from an opencv image to QPixmap"""
        rgb_image = cv2.cvtColor(cv_img, cv2.COLOR_BGR2RGB)
        h, w, ch = rgb_image.shape
        bytes_per_line = ch * w
        convert_to_Qt_format = QtGui.QImage(rgb_image.data, w, h, bytes_per_line, QtGui.QImage.Format_RGB888)
        p = convert_to_Qt_format.scaled(self.display_width, self.display_height, Qt.KeepAspectRatio)
        return QPixmap.fromImage(p)

def signal_handler(sig, frame):
    print('')
    sys.exit(0)

def create_app(argv=[]):
    signal.signal(signal.SIGINT, signal_handler)
    app = QApplication(argv)
    win = FaceRecognition()                    
    win.show()
    return app, win

def main():
    app, _win = create_app(sys.argv)                      
    return app.exec_()               

if __name__ == '__main__':              
    sys.exit(main())
