#coding=utf8
import numpy as np
import cv2
from matplotlib import pyplot as plt
import math

def match (tar_img,tar_des,tar_kp, item_img, item_des,item_kp):
	# FLANN parameters
	FLANN_INDEX_KDTREE = 0
	index_params = dict(algorithm = FLANN_INDEX_KDTREE, trees = 5)
	search_params = dict(checks=50)   # or pass empty dictionary

	flann = cv2.FlannBasedMatcher(index_params,search_params)

	matches = flann.knnMatch(item_des,tar_des,k=2)

	# Need to draw only good matches, so create a mask
	# matchesMask = [[0,0] for i in xrange(len(matches))]
	# ratio test as per Lowe's paper
	matches_good = []
	for i,(m,n) in enumerate(matches):
		if m.distance < 0.9*n.distance:
			matches_good.append(m)
			#matchesMask[i]=[1,0]

	print tar_img.shape
	print item_img.shape

	SUPPORTSIGMA = 0.2
	revSUPPORTSIGMA2 = 0.5/(SUPPORTSIGMA*SUPPORTSIGMA)
	w = tar_img.shape[1] + item_img.shape[1]
	h = tar_img.shape[0] + item_img.shape[0]
	print w,h,revSUPPORTSIGMA2
	hough_map = np.zeros((w,h), dtype=np.float32)

	for i,m in enumerate(matches_good):
		loc_x = int(tar_kp[m.trainIdx].pt[1] - item_kp[m.queryIdx].pt[1] + item_img.shape[1])
		loc_y = int(tar_kp[m.trainIdx].pt[0] - item_kp[m.queryIdx].pt[0] + item_img.shape[0])
		
		#hough_map[loc_x,loc_y] = hough_map[loc_x,loc_y] + math.exp(-revSUPPORTSIGMA2*m.distance*m.distance)
		hough_map[loc_x,loc_y] = hough_map[loc_x,loc_y] + 1
        #print loc_x
		print loc_x,loc_y, hough_map[loc_x,loc_y],m.distance
		
	for i, m in enumerate(matches_good) :
		loc_x = int(tar_kp[m.trainIdx].pt[1] - item_kp[m.queryIdx].pt[1] + item_img.shape[1])
		loc_y = int(tar_kp[m.trainIdx].pt[0] - item_kp[m.queryIdx].pt[0] + item_img.shape[0])

		cnt = hough_map[loc_x,loc_y]

		if(cnt > 0):
			#pt1 = (loc_y - item_img.shape[0],loc_x - item_img.shape[1])
			pt2 = (loc_x,loc_y)
			pt1 = (loc_x - item_img.shape[1],loc_y - item_img.shape[0])
            
            
            #print pt1,pt2
			cv2.rectangle(tar_img,pt2,pt1,(255,255,0),2)
			hough_map[loc_x,loc_y] = 0;
    
    #img3 = cv2.drawMatchesKnn(img1,kp1,img2,kp2,matches,None,**draw_params)

    #cv2.imshow("img3",img3);
	cv2.imshow("img",tar_img)
	#cv2.imshow("item",item_img)
	cv2.waitKey(0)

	#plt.imshow(tar_img,),plt.show()