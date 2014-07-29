#coding=utf8
import proc
import cv2
import numpy as np
import os, sys
from datetime import datetime
from matplotlib import pyplot as plt
import matplotlib.patches as mpatches
from skimage import morphology
from skimage.color import label2rgb
from skimage.measure import regionprops

def process_img (timestamp,img, point):
    img_gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
    thr = cv2.adaptiveThreshold(img_gray,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv2.THRESH_BINARY,11,2)
    
    thr = 255 - thr
    kernel = np.ones((1,7),np.uint8)
    dilated_img = cv2.dilate(thr, kernel,iterations = 1)
    L = morphology.label(dilated_img)
    #cv2.circle(img,point,5,(255,0,255),3)
    #cv2.imshow('img',img)
    
    cnt = 1
    for region in regionprops(L):
        minr, minc, maxr, maxc = region.bbox
        
        if maxr - minr > 200:
            continue

        if point[1]>=minr and point[1]<=maxr and point[0]>=minc and point[0]<=maxc:
            subimg = img[minr:maxr,minc:maxc]
            
            imgName = 'sub/' + timestamp + '_' + str(cnt) + '.png'
            cnt = cnt + 1
            cv2.imwrite(imgName,subimg)
            #cv2.imshow('sub',subimg)
            #if cv2.waitKey(0) == 27:
            #    sys.exit(0)

if __name__ == '__main__':
    logDir = 'C:\\Users\\LingFeng\\Desktop\\hengtian\\log/'
    screenDir = logDir + 'screen/'
    mr,clickAccessInfo = proc.recordFromMouseLog(logDir)

    for r in mr :
        if r[1] == 'MOUSE LEFT DOWN':
            imgPath = screenDir + r[0] + '.png'
            a = max(r[4][0],0)
            b = max(r[4][1],0)
            x = r[2][0] - a 
            y = r[2][1] - b

            if os.path.exists(imgPath) == False or r[6] != 'chrome.exe':#'EXCEL.EXE':
                continue
            
            #if clickAccessInfo.has_key(r[0]) and clickAccessInfo[r[0]][0] != 'pane':
            #    print clickAccessInfo[r[0]]
            #    continue
            
            f = open('acc/' + r[0] + '.txt','w')
            f.write(clickAccessInfo[r[0]][0] + '\n')
            f.write(clickAccessInfo[r[0]][1] + '\n')
            f.write(clickAccessInfo[r[0]][2] + '\n')
            f.close()

            img = cv2.imread(imgPath)
            process_img(r[0],img,(x,y))