from copy import deepcopy

import numpy as np
import cv2


# ЛР3 - границы объектов
# если в пикселе сильно (быстро) меняется цвет (функция) -> производная, значит это граница
# градиент: (di/dx; di/dy)
# большой градиент - граница
# алгоритм Канни
# 1. ч.б.
# 2. Gauss Blur
# 3. Градиенты
# 4. (тоже 3 пункт) Численные методы (матан для натуральных чисел, производная двух переменных)
# Метод Собеля
# две матрицы свёртки к каждому пикселю 2 раза
# находим длину вектора для каждого пикселя и сравниваем с другими
# считаем tg=Gy/Gx = 45 град
# 4. Подавление немаксимумов
# если величина градиента больше чем у соседей, то это граница
# все что граница - черным, не граница - белым
# 5. Двойная пороговая фильтрация (применяем только к границам)
# если градиент больше high - точно граница, меньше чем low - Точно не граница
# если попали по-середине, смотрим, есть ли вокруг него граница
# если да, то текущий граница, иначе нет
# 6. контуры надо замкнуть, разбирать не будем

def getPicture(path=r'.\fruit.jpg'):
    return cv2.imread(path, cv2.IMREAD_ANYDEPTH)


img = cv2.GaussianBlur(getPicture(), (5, 5), 1.4)
n, m = img.shape[:2]
n = int(n)
m = int(m)
print(n, m)


def some_xy():
    gx = np.array([
        [-1, 0, 1],
        [-2, 0, 2],
        [-1, 0, 1],
    ])

    gy = np.array([
        [-1, -2, -1],
        [0, 0, 0],
        [1, 2, 1],
    ])

    imgNew1 = np.zeros((n, m))
    imgNew2 = np.zeros((n, m))

    for i in range(1, n - 1):
        for j in range(1, m - 1):
            subImg = img[i - 1:i + 2, j - 1:j + 2]
            imgNew1[i][j] = np.sum(np.multiply(subImg, gx))
            imgNew2[i][j] = np.sum(np.multiply(subImg, gy))

    return imgNew1, imgNew2


def gradient():
    gx, gy = some_xy()

    matrix_length = np.zeros((n, m))  # значения градиентов
    matrix_atan = np.zeros((n, m))  # тангенсы (направления от 0 до 7)

    for i in range(1, n - 1):
        for j in range(1, m - 1):
            x = int(gx[i][j])
            y = int(gy[i][j])
            matrix_length[i][j] = (x ** 2 + y ** 2) ** 0.5

            tg = -1
            if x != 0:
                tg = y / x

            value = -1

            if x > 0 and y < 0 and tg < -2.414 or \
                    x < 0 and y < 0 and tg > 2.414:
                value = 0
            elif x > 0 and y < 0 and tg < -0.414:
                value = 1
            elif x > 0 and y < 0 and tg > -0.414 or \
                    x > 0 and y > 0 and tg < 0.414:
                value = 2
            elif x > 0 and y > 0 and tg < 2.414:
                value = 3
            elif x > 0 and y > 0 and tg > 2.414 or \
                    x < 0 and y > 0 and tg < -2.414:
                value = 4
            elif x < 0 and y > 0 and tg < -0.414:
                value = 5
            elif x < 0 and y > 0 and tg > -0.414 or \
                    x < 0 and y < 0 and tg < 0.414:
                value = 6
            elif x < 0 and y < 0 and tg < 2.414:
                value = 7

            matrix_atan[i][j] = value

    # max_matrix_length = np.max(matrix_length)
    # low_level = max_matrix_length // 25
    # high_level = max_matrix_length // 10
    low_level = 20
    high_level = 60

    matrix_border = deepcopy(img)  # границы
    for i in range(1, n - 1):
        for j in range(1, m - 1):
            way_plus = [[-1, -1], [-1, -1]]
            some = matrix_atan[i][j]
            # [y, x] logic
            if some == 0 or some == 4:
                way_plus = [[-1, 0], [1, 0]]
            elif some == 2 or some == 6:
                way_plus = [[0, -1], [0, 1]]
            elif some == 1 or some == 5:
                way_plus = [[-1, 1], [1, -1]]
            elif some == 3 or some == 7:
                way_plus = [[1, 1], [-1, -1]]

            grad = matrix_length[i][j]

            if grad >= matrix_length[i + way_plus[0][0]][j + way_plus[0][1]] \
                    and grad >= matrix_length[i + way_plus[1][0]][j + way_plus[1][1]]:
                matrix_border[i][j] = 0
            else:
                matrix_border[i][j] = 255

            if matrix_border[i][j] == 0:
                matrix_border[i][j] = 255
                subImg = matrix_border[i - 1:i + 2, j - 1:j + 2]
                min_el = np.min(subImg)

                if grad < low_level:
                    matrix_border[i][j] = 255
                elif grad > high_level:
                    matrix_border[i][j] = 0
                elif min_el == 0:
                    matrix_border[i][j] = 0

    cv2.imshow("nameee", matrix_border)
    cv2.waitKey(0)


gradient()
