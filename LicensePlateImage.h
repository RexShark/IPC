#ifndef LICENSE_PLATE_DETECTOR_H
#define LICENSE_PLATE_DETECTOR_H

#include<opencv2/opencv.hpp>
#include<tesseract/baseapi.h>
#include<iostream>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/msg.h>
#include<cstring>

#define MSG_KEY 1234
#define MAX_MSG_SIZE 512

using namespace std;
using namespace cv;

struct message_buffer{
	long msg_type;
	char msg_text[MAX_MSG_SIZE];
};
class LicensePlateDetector{
        public:
        LicensePlateDetector();
        Mat preprocessImage(Mat& image);
        Rect detectLicensePlate(Mat& edgeDetected);
//      string recognizeLicensePlate(const Mat& licensePlate);
        private:
        tesseract :: TessBaseAPI ocr;
};

#endif
