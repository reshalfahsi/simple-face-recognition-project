#include "FaceRecognition/FaceRecognition.h"

using namespace face_recognition;

FaceRecognition::FaceRecognition(){

    img_result = cv::Mat::zeros(cv::Size(640, 480), CV_64FC1);

}

FaceRecognition::~FaceRecognition(){


}

void FaceRecognition::start(){

    run = true;
    thread = std::thread(&FaceRecognition::loop, this);

}

void FaceRecognition::loop(){

    cv::VideoCapture cap(0);
    if (!cap.isOpened())
    {
        run = false;
	std::cout << "Cannot open the video cam" << std::endl;
	return;
    }
    
    double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "Frame Size:" << dWidth << "x" << dHeight << std::endl;

    cv::Mat frame;

    while(run){
      
	cap >> frame;
        
        if (frame.empty())
	{
	    std::cout << "Cannot read a frame video stream" << std::endl;
            run = false;
	    break;
        }

        img_result = frame.clone();

    }


}
