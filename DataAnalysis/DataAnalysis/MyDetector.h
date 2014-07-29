#pragma once
#include <stdio.h>
#include "opencv2/core/core.hpp"
#include <opencv2/contrib/contrib.hpp>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv/cv.h"
#include <vector>

using namespace std;
using namespace cv;

class MyImage
{
public:
	Mat img;
	Mat img_gray;
	Mat img_canny;
	vector<KeyPoint> img_kps;
	Mat img_des;

public:
	//static MyImage loadImage(string imgPath);
};

class MyDetector
{
public:
	const static int HESSIAN_THRESHOLD = 400;
	const static int OCTAVES = 4;
	const static int OCTAVELAYERS = 4;
	const static bool EXTENDED = false;
	const static bool UPRIGHT = false;
	const static int FAST_CORNER_THRESH = 10;
	const static bool FAST_CORNER_THRESH_SUPRESS = false;
	const static int kernel_size  = 3;
	const static int lowThreshold = 18;
	const static int ratio        = 3;

public:
	MyDetector(void);
	~MyDetector(void);

	void init_surf_inst();
	MyImage loadImage(string imgPath);
	MyImage loadImage(string imgPath, Rect rect);
	
	void initImage(string imgPath);
	void loadItems(string fold);
	void loadItems2(string fold);
	void addItem(string itemPath);

	bool cannyDetect(Mat tar, Mat skel);
	vector<Rect> clusterDetects_hough_fixedscale(MyImage& tar_img,MyImage item_img, vector<DMatch > matches);
	vector<Rect> clusterDetects_hough(MyImage& tar_img, MyImage& item_img,vector<DMatch> matches);
	vector<Rect> itemMatch(string tar_img_path, string item_img_path);
	vector<Rect> itemMatch(MyImage& tar_img, MyImage& item_img, int method=1);
	void itemAllMatch();

	void test();
public:
	SURF surf_inst;
	SIFT sift_inst;

	MyImage myImage;
	vector<MyImage> items;


};

