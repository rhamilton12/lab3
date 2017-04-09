#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
 
int main() {
	
	VideoCapture stream1(0);  		 
 
	if (!stream1.isOpened())  		
	{
		cout << "cannot open camera";
	}
 
//loop to keep capturing images

	while (true) {
			Mat cameraFrame;
			stream1.read(cameraFrame);
			imwrite("/media/card/capture_Image/image.png", cameraFrame);
			imshow("cam", cameraFrame);
			if (waitKey(30) >= 0)
			break;
		     }
	return 0;
}
