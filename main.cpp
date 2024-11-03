#include "LicensePlateImage.h"

using namespace std;
using namespace cv;


LicensePlateDetector::LicensePlateDetector(){
                if(ocr.Init(NULL,"eng", tesseract :: OEM_DEFAULT)){
                        cerr<<"Error: Could not initialize Tesseract!"<<endl;
                }
                ocr.SetVariable("tessedit_char_whitelist", "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
                ocr.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
}

Mat LicensePlateDetector :: preprocessImage(Mat& image){

    Mat gray, blurred, edgeDetected;
    cvtColor(image, gray, COLOR_BGR2GRAY);  
    GaussianBlur(gray, blurred, Size(5, 5), 0);  
    Canny(blurred, edgeDetected, 100, 200);  
    return edgeDetected;
}

Rect LicensePlateDetector :: detectLicensePlate(Mat & edgeDetected){

    vector<vector<Point>> contours;
    findContours(edgeDetected, contours, RETR_TREE, CHAIN_APPROX_SIMPLE);

    for (const auto& contour : contours) {
        Rect boundingRec = boundingRect(contour);

        double perimeter = arcLength(contour , true);

       if (boundingRec.width > boundingRec.height && boundingRec.area() > 500 && (static_cast<double>(boundingRec.width) / boundingRec.height > 2)&&(perimeter > 100 && perimeter < 400)) {
            return boundingRec;  
        }
    }
    return Rect();  
}


int main(int argc,char** argv){
	if(argc<2){
		cerr<<"Error: No image provided!";
		return -1;
	}
	
	//Load image

	Mat carImage = imread(argv[1]);

	if(carImage.empty()){
		cerr<<"Error: Could not load image!"<<endl;
		return -1;
	}

	//create an instance of LicensePlateDetection 
	
	LicensePlateDetector detector;
	Mat edgeDetected = detector.preprocessImage(carImage);

	Rect LicensePlateRegion = detector.detectLicensePlate(edgeDetected);

	if(LicensePlateRegion.area() == 0){
		cerr<<"Error: No license Plate detected!"<<endl;
		return -1;
	}

	//Draw rectangle on original image
	rectangle(carImage, LicensePlateRegion, Scalar(0,255,0),2);
	imshow("Detected license plate", carImage);
	//waitKey(0);

	//Extract the license plate region 
	Mat licensePlate = carImage(LicensePlateRegion);

	//Save the image
	
	string savedImagePath = "/home/user/Raksha/project2/extracted_license_plate.jpeg";
	imwrite(savedImagePath, licensePlate);
        cout<<"Client : Saved license Plate image to : "<< savedImagePath<<endl;

        //create message queue
        int msg_queue_id = msgget(MSG_KEY, IPC_CREAT | 0666);
        if(msg_queue_id == -1){
                cerr<<"Error: Could not create message queue"<<endl;
                return -1;
        }

        //prepare the message to send to the server
        message_buffer msg;
        msg.msg_type =1;
        strncpy(msg.msg_text, savedImagePath.c_str(), MAX_MSG_SIZE -1);
        msg.msg_text[MAX_MSG_SIZE -1] ='\0';

        if(msgsnd(msg_queue_id, &msg, strlen(msg.msg_text) +1, 0) == -1){
                cerr<<"Error: Failed to send message"<<endl;
                return -1;
        }
        cout<<"Client: Sent image PATH TO server: "<<savedImagePath<<endl;

        return 0;


}


