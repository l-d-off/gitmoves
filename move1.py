import cv2


def picture():
    name = "fruit.jpg"
    img = cv2.imread(name)
    cv2.namedWindow("fruit", cv2.WINDOW_NORMAL)
    # чб
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    cv2.imshow('fruit', gray)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


def video():
    name = "output.mov"
    cap = cv2.VideoCapture(name, cv2.CAP_ANY)
    while True:
        ret, frame = cap.read()
        if not ret:
            cv2.waitKey(0)
            break
        cv2.imshow('video', frame)
        if cv2.waitKey(1) & 0xFF == 27:  # 0xFF ждёт escape
            break
    cap.release()
    cv2.destroyAllWindows()


def webcam(cam=0):
    # 0 - computer, 2 - phone
    video = cv2.VideoCapture(cam)
    w = int(video.get(cv2.CAP_PROP_FRAME_WIDTH))
    h = int(video.get(cv2.CAP_PROP_FRAME_HEIGHT))
    fourcc = cv2.VideoWriter_fourcc(*'XVID')
    video_writer = cv2.VideoWriter("output.mov", fourcc, 25, (w, h))
    while True:
        ok, img = video.read()
        cv2.imshow('webcam', img)
        video_writer.write(img)
        if cv2.waitKey(1) & 0xFF == 27:
            break
    video.release()
    cv2.destroyAllWindows()
