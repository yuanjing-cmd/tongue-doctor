# 图像颜色分布计算,需要图像只保留要比较的部分
import sys

import cv2
import os
import numpy as np
os.chdir(sys.path[0])

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


# 比例
def Proportion(x, all):
    x = x / all
    return x


filePath = r"C:\Users\lyz13\Desktop\tongue_system\datasets\tongue_hsv\images/"
txtPath = r"C:\Users\lyz13\Desktop\tongue_system\datasets\tongue_hsv\labels/"
hahah = [[0] * 7 for _ in range(5)]
jishu = 0
jishu1 = [0] * 5
allcolor1 = [[0] * 7 for _ in range(150)]
for filename in os.listdir(filePath):
    print(f"第{jishu}张")
    img = cv2.imread(filePath + filename)
    allcolor = [0] * 6

    for i in range(len(img)):
        for j in range(len(img[0])):
            h1,s1,v1 = rgb2hsv(img[i][j][0], img[i][j][1], img[i][j][2])
            if int(s1)==0 and int(v1) == 0:
                continue
            allcolor[int(h1 / (360 / 6))] += 1
    # allcolor.sort()
    for i in range(len(allcolor)):
        x = Proportion(allcolor[i], sum(allcolor))
        allcolor1[jishu][i] = x

    with open(txtPath + filename.replace(".jpg", ".txt")) as ff:
        dl = ff.readlines()
        dl = str(dl)
    allcolor1[jishu][6] = dl[2]
    print(allcolor1[jishu])
    jishu += 1

allcolor1 = np.array(allcolor1)
np.save('hsv.npy', allcolor1)
