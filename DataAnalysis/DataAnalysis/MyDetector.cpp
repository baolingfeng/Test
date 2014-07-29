#include "MyDetector.h"
#include <io.h>
#include "record.h"

MyDetector::MyDetector(void)
{
	init_surf_inst();
}


MyDetector::~MyDetector(void)
{
}

void MyDetector::init_surf_inst()
{
	surf_inst.hessianThreshold = HESSIAN_THRESHOLD;
	surf_inst.nOctaves         = OCTAVES;
	surf_inst.nOctaveLayers    = OCTAVELAYERS;
	surf_inst.extended         = EXTENDED;
	surf_inst.upright          = UPRIGHT;
}


MyImage MyDetector::loadImage(string imgPath)
{
	Mat img = imread(imgPath,1);
	Mat img_gray, img_canny;
	cvtColor(img,img_gray,COLOR_BGR2GRAY);
	Canny(img_gray,img_canny,lowThreshold, lowThreshold*ratio, kernel_size, false);
	
	vector<KeyPoint> img_kps;
	Mat img_des;

	//sift_inst.detect(img_gray,img_kps);
	//sift_inst.compute(img_gray,img_kps,img_des);

	FAST(img_gray, img_kps, FAST_CORNER_THRESH, FAST_CORNER_THRESH_SUPRESS ); //key point detector
	surf_inst(img_gray, Mat(), img_kps, img_des, true);

	MyImage myImg;
	myImg.img = img;
	myImg.img_gray = img_gray;
	myImg.img_canny = img_canny;
	myImg.img_kps = img_kps;
	myImg.img_des = img_des;

	return myImg;
}

MyImage MyDetector::loadImage(string imgPath, Rect rect)
{
	Mat src_img = imread(imgPath,1);
	Mat img = src_img(rect);

	Mat img_gray, img_canny;
	cvtColor(img,img_gray,COLOR_BGR2GRAY);
	Canny(img_gray,img_canny,lowThreshold, lowThreshold*ratio, kernel_size, false);
	
	vector<KeyPoint> img_kps;
	Mat img_des;

	//sift_inst.detect(img_gray,img_kps);
	//sift_inst.compute(img_gray,img_kps,img_des);

	FAST(img_gray, img_kps, FAST_CORNER_THRESH, FAST_CORNER_THRESH_SUPRESS ); //key point detector
	surf_inst(img_gray, Mat(), img_kps, img_des, true);

	MyImage myImg;
	myImg.img = img;
	myImg.img_gray = img_gray;
	myImg.img_canny = img_canny;
	myImg.img_kps = img_kps;
	myImg.img_des = img_des;

	return myImg;
}

void MyDetector::initImage(string imgPath)
{
	myImage = loadImage(imgPath);
}

void MyDetector::loadItems2(string fold)
{
	string temp; 
	if(fold[fold.length()-1] == '/')
	{
		temp = fold + "*";
	}
	else
	{
		temp = fold + "/*";
		fold = fold + "/";
	}

	_finddata_t fileInfo;
	long lfDir;
	if((lfDir = _findfirst(temp.c_str(),&fileInfo)) == -1)
	{

	}
	else
	{
		do
		{
			if (!(fileInfo.attrib & _A_SUBDIR))  
			{
				string fileName(fileInfo.name);

				int idx = fileName.find(".png");
				int idx2 = fileName.find("mask");
				if(idx<0 || idx2>=0)
					continue;

				

				addItem(fold+fileName);
				cout<<fileInfo.name<<endl;
			}
			else
			{
				if(strcmp(fileInfo.name,".") != 0 && 
					strcmp(fileInfo.name,"..") != 0)
				{
					cout<<fileInfo.name<<endl;
					loadItems2(fold+fileInfo.name);
				}
			}
		
		}while( _findnext( lfDir, &fileInfo ) == 0 );
	}
}

void MyDetector::loadItems(string fold)
{
	string temp; 
	if(fold[fold.length()-1] == '/')
	{
		temp = fold + "*";
	}
	else
	{
		temp = fold + "/*";
		fold = fold + "/";
	}
	
	_finddata_t fileDir;
	long lfDir;
	if((lfDir=_findfirst(temp.c_str(),&fileDir)) == -1)
	{
		cout<<"not a fold"<<endl;
	}
	else
	{
		do{
			string temp(fileDir.name);
			if(temp.find(".png")<0) continue;

			cout << temp <<endl;	
			addItem(fold+temp);
        }while( _findnext( lfDir, &fileDir ) == 0 );
	}

}

void MyDetector::addItem(string itemPath)
{
	MyImage item = loadImage(itemPath);
	items.push_back(item);
}

bool MyDetector::cannyDetect(Mat tar, Mat skel)
{
	if(tar.rows != skel.rows || tar.cols != skel.cols) return false;
	
	int count = 0;
	for(int i=0; i<skel.rows; i++)
	{
		for(int j=0; j<skel.cols; j++)
		{
			if(skel.at<uchar>(i,j) == 255 && tar.at<uchar>(i,j) == 255)
			{
				count++;
			}
		}
	}

	double ratio = count * 1.0 / countNonZero(skel);

	//cout<<"edge: "<<ratio<<endl;

	return ratio>0.6;
}

vector<Rect> MyDetector::clusterDetects_hough_fixedscale(MyImage& tar_img,MyImage item_img, vector<DMatch > matches)
{
	const double SUPPORTSIGMA = 0.2;
	double revSUPPORTSIGMA2 = 0.5/(SUPPORTSIGMA*SUPPORTSIGMA);

	int i;
	int fr_i, to_i;
	int loc_x, loc_y;
	float cnt;
	Mat houghmap;
	houghmap = Mat::zeros(tar_img.img.rows+item_img.img.rows, tar_img.img.cols+item_img.img.cols, CV_32FC1);

	vector<cv::Rect> detects;
	for( i=0; i<matches.size(); i++)
	{
		fr_i = matches[i].queryIdx;
		to_i = matches[i].trainIdx;

		loc_x = tar_img.img_kps[to_i].pt.x - item_img.img_kps[fr_i].pt.x + item_img.img.cols;
		loc_y = tar_img.img_kps[to_i].pt.y - item_img.img_kps[fr_i].pt.y + item_img.img.rows;

		//add support to the location
		//houghmap.at<float>(loc_y, loc_x) += 1;
		houghmap.at<float>(loc_y, loc_x) += exp( -revSUPPORTSIGMA2*matches[i].distance*matches[i].distance );
	}

	//printf("\nFunction clusterDetects_hough_fixedscale\n");
	for( i=0; i<matches.size(); i++)
	{
		fr_i = matches[i].queryIdx;
		to_i = matches[i].trainIdx;

		loc_x = tar_img.img_kps[to_i].pt.x - item_img.img_kps[fr_i].pt.x + item_img.img.cols;
		loc_y = tar_img.img_kps[to_i].pt.y - item_img.img_kps[fr_i].pt.y + item_img.img.rows;
		
		cnt = houghmap.at<float>(loc_y, loc_x);

		//cout<<cnt<<" with threshold: "<<0.01*item_img.img_kps.size()<<endl;
		if( cnt > 0)// 0.01*item_img.img_kps.size()) //0.2 * item_kps.size() )//-----------------------tuning term----------------
		{

			Rect temp( loc_x-item_img.img.cols, loc_y-item_img.img.rows, item_img.img.cols, item_img.img.rows) ;
			if(temp.x<0) temp.x = 0;
			if(temp.y<0) temp.y = 0;
			if(temp.x+temp.width > tar_img.img.cols) temp.width = tar_img.img.cols - temp.x;
			if(temp.y+temp.height > tar_img.img.rows) temp.height = tar_img.img.rows - temp.y;

			if(cannyDetect(tar_img.img_canny(temp),item_img.img_canny))
			{
				detects.push_back( temp );
			}

			houghmap.at<float>(loc_y, loc_x) = 0;
		}
		
	}
	return detects;
}

vector<Rect> MyDetector::clusterDetects_hough(MyImage& tar_img, MyImage& item_img,vector<DMatch> matches)
{
	printf("\nFunction clusterDetects_hough\n");
	const double MINSCALE = 0.5;
	const double MAXSCALE = 5;
	const double FINENESS = 0.1;
	const double SUPPORTSIGMA = 0.2;
	double ep = 1e-5;//EPSILON;
	double revFINENESS = 1.0/FINENESS;
	double revSUPPORTSIGMA2 = 0.5/(SUPPORTSIGMA*SUPPORTSIGMA);

	int padrows = MAXSCALE * item_img.img.rows;
	int padcols = MAXSCALE * item_img.img.cols;
	int sz[] = {tar_img.img.rows+padrows, tar_img.img.cols+padcols, (MAXSCALE-MINSCALE)/FINENESS+1  };
	SparseMat_<float> houghmap(3, sz);

	int i,j;
	int fr[2];
	int to[2];
	Point2f fr_pt[2];
	Point2f to_pt[2];
	double fr_dx, fr_dy;
	double to_dx, to_dy;

	vector<Rect> detects;
	int nDetects;
	
	int    loc_x, loc_y, loc_s;
	double org_x, org_y, org_s;

	for( i=0; i<matches.size(); i++ )
	{
		for( j=0; j<matches.size(); j++ )
		{
			if(i==j)
			{
				continue;
			}
			fr[0] = matches[i].queryIdx;
			to[0] = matches[i].trainIdx;

			fr_pt[0] = item_img.img_kps[fr[0]].pt;
			to_pt[0] = tar_img.img_kps[to[0]].pt;

			fr[1] = matches[j].queryIdx;
			to[1] = matches[j].trainIdx;

			fr_pt[1] = item_img.img_kps[fr[1]].pt;
			to_pt[1] =  tar_img.img_kps[to[1]].pt;

			fr_dx = fr_pt[0].x - fr_pt[1].x;
			fr_dy = fr_pt[0].y - fr_pt[1].y;
			to_dx = to_pt[0].x - to_pt[1].x;
			to_dy = to_pt[0].y - to_pt[1].y;

			if( abs(fr_dx+fr_dy)<ep || abs(fr_dx)<ep || abs(fr_dy)<ep )
			{
				continue;
			}

			org_s = (to_dx+to_dy)/(fr_dx+fr_dy);
			if( org_s < MINSCALE || org_s > MAXSCALE )
			{
				continue;
			}
			org_x = 0.5*(to_pt[0].x+to_pt[1].x - org_s*(fr_pt[0].x+fr_pt[1].x));
			org_y = 0.5*(to_pt[0].y+to_pt[1].y - org_s*(fr_pt[0].y+fr_pt[1].y));

			loc_x = (int)floor(org_x+padcols+0.5);
			loc_y = (int)floor(org_y+padrows+0.5);
			loc_s = (int)floor((org_s-MINSCALE)*revFINENESS + 0.5);

			houghmap.ref(loc_y, loc_x, loc_s) += exp( -revSUPPORTSIGMA2*( matches[i].distance*matches[i].distance + 
																		 matches[j].distance*matches[j].distance ) );
		}
	}

	nDetects = 0;
	
	SparseMatIterator_<float> it = houghmap.begin(),
                              it_end = houghmap.end();
    for(; it != it_end; ++it)
	{
		const SparseMat_<float>::Node* nd = it.node();
		loc_y = nd->idx[0] - padrows;
		loc_x = nd->idx[1] - padcols;
		org_s = (nd->idx[2])*FINENESS + MINSCALE;

		printf("Rect_%d (%4d,%4d,%2.1lf) contains %.2f matches.\n", nDetects, loc_x, loc_y, org_s, houghmap(nd->idx));

		//float itnum_mat = houghmap(nd->idx[0],nd->idx[1], nd->idx[2]);//debug
		if( (*it) > 0.0005 * item_img.img_kps.size() * item_img.img_kps.size())
		{
			
			Rect temp( loc_x, loc_y, org_s*item_img.img.cols, org_s*item_img.img.rows);
			if(temp.x<0) temp.x = 0;
			if(temp.y<0) temp.y = 0;
			if(temp.x+temp.width>tar_img.img.cols) temp.width = tar_img.img.cols - temp.x;
			if(temp.y+temp.height>tar_img.img.rows) temp.width = tar_img.img.rows - temp.y;

			detects.push_back(temp);
			nDetects ++;
		}
	}

	cout<<item_img.img_kps.size()<<"/"<<0.001 * item_img.img_kps.size() * item_img.img_kps.size()<<endl;

	return detects;
}


vector<Rect> MyDetector::itemMatch(MyImage& tar_img, MyImage& item_img, int method)
{
	FlannBasedMatcher		 matcher;
	vector<vector<DMatch > > matches;
	vector<DMatch >			 matches_good;
	vector<DMatch >			 matches_all;
	Mat						 matches_img;

	vector<Rect> detects;

	if(tar_img.img_des.empty()) return detects;

	matcher.knnMatch(item_img.img_des, tar_img.img_des, matches, 2);
	for(int i = 0; i < min(tar_img.img_des.rows-1,(int) matches.size()); i++)
	{
		if((matches[i][0].distance < 0.98*(matches[i][1].distance)) && 
			((int) matches[i].size()<=2 && (int) matches[i].size()>0))
		{
			matches_good.push_back(matches[i][0]);
		}
		matches_all.push_back(matches[i][0]);
	}

	drawMatches( item_img.img_gray, item_img.img_kps, tar_img.img_gray,tar_img.img_kps, matches_good, matches_img, 
 		Scalar::all(-1), Scalar::all(-1), vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

	imwrite("matches.png",matches_img);
	//imshow("matches",matches_img);
	//waitKey(0);

	
	if(method == 1)
	{
		detects = clusterDetects_hough_fixedscale(tar_img,item_img,matches_good);
	}
	else if(method == 2)
	{
		detects = clusterDetects_hough(tar_img,item_img,matches_good);
	}

	/*
	for(int i=0; i<detects.size(); i++)
	{
		rectangle(tar_img.img,detects[i],Scalar(0,0,255),2);
	}
	*/

	return detects;
}

void MyDetector::itemAllMatch()
{
	for(int i=0; i<items.size(); i++)
	{
		itemMatch(myImage, items[i]);
	}
}

vector<Rect> MyDetector::itemMatch(string tar_img_path, string item_img_path)
{
	MyImage tar_img = loadImage(tar_img_path);
	MyImage item_img = loadImage(item_img_path);

	return itemMatch(tar_img, item_img);

	//imshow("detects",tar_img.img);
	//imwrite("image.png",tar_img.img);
	//imwrite("image_canny.png",tar_img.img_canny);
	//waitKey(0);
}

void MyDetector::test()
{
	//string imgPath = "D:/temp/tmp.png";
	string imgPath = "C:/Users/LingFeng/Desktop/Release/log_6/screen/2014-06-24-16-12-22-853.png"; //2014-06-24-15-30-11-525.png";
	//initImage(imgPath);

	//string itemPath = "C:/Users/LingFeng/Documents/Visual Studio 2010/Projects/CVTool2/tpls/standard/Chrome/ROW_4/google_search.png";
	string itemPath = "D:/temp/refresh.png";
	//addItem(itemPath);

	//string itemPath = "C:/Users/LingFeng/Documents/Visual Studio 2010/Projects/CVTool2/tpls/standard/TaskBar/ROW_1";
	//loadItems2(itemPath);

	itemMatch(imgPath,itemPath);
	//itemAllMatch();

	
}

/*
int main_1()
{
	string logDir = "C:\\Users\\LingFeng\\Desktop\\Release\\log_6\\";

	EventRecord r;
	r.timestamp = "2014-06-24-15-45-22-751";
	r.mousePosition = Point(109,640);
	r.windowRect = Rect(0,85,1280,984-85);

	string imgPath = logDir+"screen\\"+r.timestamp+".png";
	cout<<imgPath<<endl;
	Mat img = imread(imgPath);
	if(img.empty()) return 0;
			
	Point pt;
	pt.x = r.mousePosition.x - r.windowRect.x;
	pt.y = r.mousePosition.y - r.windowRect.y;

	Rect rect;
	if(pt.x-100>0)
	{
		rect.x = pt.x - 100;
		pt.x = 100;
	}
	else
	{
		rect.x = 0;
	}
			
	if(pt.y-100>0)
	{
		rect.y = pt.y-100;
		pt.y = 100;
	}
	else
	{
		rect.y = 0;
	}
			
	rect.width = rect.x+200>img.cols ? img.cols-rect.x : 200;
	rect.height = rect.y+200>img.rows ? img.rows-rect.y : 200;

	MyDetector dect;
	string itemImgPath = "D:/temp/tabclose.png";
	//MyImage myImg = dect.loadImage(imgPath,rect);
	MyImage myImg = dect.loadImage(imgPath);
	MyImage item = dect.loadImage(itemImgPath);
	
	imshow("sub",myImg.img);
	waitKey(0);

	vector<Rect> detects = dect.itemMatch(myImg,item);
			
	if(detects.size()<=0) return 0;

	for(int j=0; j<detects.size(); j++)
	{
		rectangle(myImg.img,detects[j],Scalar(0,255,0));
	}

	circle(myImg.img,pt,5,Scalar(255,255,0),2);

	imshow("img",myImg.img);
	waitKey(0);

	//MyDetector dect;
	//dect.test();

	return 1;
}
*/