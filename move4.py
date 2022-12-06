from copy import deepcopy

import numpy as np
import cv2


def fun():
    video = cv2.VideoCapture(r"C:/Users/Vladislav/Desktop/university/Zhuk/zhuk1/main_video.mov", cv2.CAP_ANY)
    check, frame = video.read()
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = cv2.GaussianBlur(gray, (5, 5), 1.3)
    w = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    h = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    status = 0
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    video_writer = cv2.VideoWriter("output.mov", fourcc, 25, (w, h))

    while check:
        frame_old = gray
        check, frame = video.read()
        print(check)
        if not check:
            break
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        gray = cv2.GaussianBlur(gray, (5, 5), 1.3)
        delta_frame = cv2.absdiff(frame_old, gray)
        thresh_frame = cv2.threshold(delta_frame, 30, 255, cv2.THRESH_BINARY)[1]
        cnts, _ = cv2.findContours(thresh_frame.copy(), cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

        for contours in cnts:
            if cv2.contourArea(contours) < 1000:
                continue
            status = 1
            video_writer.write(frame)

        cv2.imshow('frame', frame)
        key = cv2.waitKey(1)
        if key == ord('q'):
            break
        print(status)

    video.release()
    cv2.destroyAllWindows()


fun()
