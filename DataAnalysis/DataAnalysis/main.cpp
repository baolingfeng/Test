#include"RecordManager.h"

#ifdef _DEBUG
	#pragma comment (lib,"opencv_core243d.lib")
	#pragma comment (lib,"opencv_features2d243d.lib")
	#pragma comment (lib,"opencv_highgui243d.lib")
	#pragma comment (lib,"opencv_imgproc243d.lib")
	#pragma comment (lib,"opencv_contrib243d.lib")
	#pragma comment (lib,"opencv_calib3d243d.lib")
	#pragma comment (lib,"opencv_flann243d.lib")
	#pragma comment (lib,"opencv_objdetect243d.lib")
	#pragma comment (lib,"opencv_nonfree243d.lib")
#endif

int main()
{
	RecordManager rm;
	rm.test();

	return 0;
}