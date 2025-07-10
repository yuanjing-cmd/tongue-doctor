import cv2
import numpy as np


def rgb2hsv(r, g, b):
    r, g, b = r/255.0, g/255.0, b/255.0
    mx = max(r, g, b)
    mn = min(r, g, b)
    df = mx-mn
    if mx == mn:
        h = 0
    elif mx == r:
        h = (60 * ((g-b)/df) + 360) % 360
    elif mx == g:
        h = (60 * ((b-r)/df) + 120) % 360
    elif mx == b:
        h = (60 * ((r-g)/df) + 240) % 360
    if mx == 0:
        s = 0
    else:
        s = df/mx
    v = mx
    return h, s, v


def Proportion(x, all):
    x = x / all
    return x


def color_similarity(filePath):
    allsq1 = [0] * 150
    a = np.load('a.npy')
    a = a.tolist()
    img = cv2.imread(filePath)
    allcolor = [0] * 6
    allcolor1 = [0] * 6
    for i in range(len(img)):
        for j in range(len(img[0])):
            h1,s1,v1 = rgb2hsv(img[i][j][0], img[i][j][1], img[i][j][2])
            if (int(s1) == 0 and int(v1)==0):
                continue
            allcolor[int(h1 / (360 / 6))] += 1
    for i in range(len(allcolor)):
        x = Proportion(allcolor[i], sum(allcolor))
        allcolor1[i] = x
    for i in range(150):
        allsq = 0
        for j in range(6):
            allsq += pow(float(a[i][j]) - allcolor1[j], 2)
            allsq /= 6
        allsq1[i] = allsq
    allsq2 = [0] * 5
    for i in range(10):
        pp = allsq1.index(min(allsq1))
        allsq2[int(a[pp][6])] += 1
        allsq1[pp] = 9999999999
    for i in range(5):
        allsq2[i] = allsq2[i] / 10
    return allsq2
