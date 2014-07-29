import numpy as np
import cv2
from matplotlib import pyplot as plt
import math

import mymatch

HESSIAN_THRESHOLD = 400
OCTAVES = 4
OCTAVELAYERS = 4
EXTENDED = False
UPRIGHT = False

item_img = cv2.imread('ticketsearch.png',1)          # queryImage
tar_img = cv2.imread('chrome2.png',1)          # trainImage

item_img_gray = cv2.cvtColor(item_img,cv2.COLOR_BGR2GRAY)
tar_img_gray = cv2.cvtColor(tar_img,cv2.COLOR_BGR2GRAY)

# Initiate detector
FAST_CORNER_THRESH = 10
FAST_CORNER_THRESH_SUPRESS = False
fast = cv2.FastFeatureDetector(FAST_CORNER_THRESH,FAST_CORNER_THRESH_SUPRESS)
surf = cv2.SURF(HESSIAN_THRESHOLD,OCTAVES,OCTAVELAYERS,EXTENDED,UPRIGHT)

#SIFT
sift = cv2.SIFT()

item_kp1 = fast.detect(item_img_gray,None)
item_kp1,item_des1 = surf.compute(item_img_gray,item_kp1)

tar_kp1 = fast.detect(tar_img_gray,None)
tar_kp1,tar_des1 = surf.compute(tar_img_gray,tar_kp1)

item_kp2, item_des2 = sift.detectAndCompute(item_img_gray,None)
tar_kp2, tar_des2 = sift.detectAndCompute(tar_img_gray,None)


print len(item_kp1), len(tar_kp1), len(item_kp2), len(tar_kp2)

mymatch.match(tar_img,tar_des1,tar_kp1,item_img,item_des1,item_kp1)
#mymatch.match(tar_img,tar_des2,tar_kp2,item_img,item_des2,item_kp2)