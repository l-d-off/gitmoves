import numpy as np
import cv2

n = 5
center = (n - 1) // 2
sigma = 1.4


def getMatrix():
    _matrix = []
    for i in range(n):
        _vector = []
        for j in range(n):
            _vector.append(gauss(i, j))
        _matrix.append(_vector)
    return _matrix


def gauss(x, y, a=center, b=center):
    return 1 / (2 * np.pi * (sigma ** 2)) * \
           np.exp(-((x - a) ** 2 + (y - b) ** 2) / (2 * sigma * sigma))


matrix = getMatrix()

def getSum():
    _sum = 0
    for i in range(n):
        for j in range(n):
            _sum += matrix[i][j]
    return _sum


sum = getSum()


def getNormMatrix():
    _matrix = []
    for i in range(n):
        _vector = []
        for j in range(n):
            _vector.append(matrix[i][j] / sum)
        _matrix.append(_vector)
    return _matrix


matrix = getNormMatrix()



sum = 0
for i in range(n):
    for j in range(n):
        sum += matrix[i][j]


def getPicture(path=r'./cat.png'):
    _img = cv2.imread(path, cv2.IMREAD_ANYDEPTH)
    return _img


def getBlurPicture(img=getPicture()):
    imgNew = img

    len1 = len(img)
    len2 = len(img[0])

    for i in range(center, len1 - center):
        for j in range(center, len2 - center):
            _sum = 0
            for q in range(n):
                for w in range(n):
                    _sum += matrix[q][w] * img[q + i - center][w + j - center]
            imgNew[i][j] = _sum

    return imgNew


out1 = getBlurPicture()
out2 = cv2.GaussianBlur(getPicture(), (5, 5), 1.4)
cv2.imshow("original", getPicture())
cv2.imshow("blur", out1)
cv2.imshow("inline blur", out2)

cv2.waitKey(0)
cv2.destroyAllWindows()
