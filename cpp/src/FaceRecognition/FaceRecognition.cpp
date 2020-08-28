#include "FaceRecognition/FaceRecognition.h"

using namespace face_recognition;

FaceRecognition::FaceRecognition(){

    img_result = cv::Mat::zeros(cv::Size(640, 480), CV_64FC1);

    auto weight_file = "../model/opencv_face_detector_uint8.pb";
    auto config_file = "../model/opencv_face_detector.pbtxt";

    net = cv::dnn::readNetFromTensorflow(weight_file, config_file);

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

    double fps, t, T;

    while(run){
      
	cap >> frame;
        
        if (frame.empty())
	{
	    std::cout << "Cannot read a frame video stream" << std::endl;
            run = false;
	    break;
        }

        t = cv::getTickCount();
        process(frame);
        T = ((double)cv::getTickCount() - t)/cv::getTickFrequency();
        fps = 1/T;
        putText(frame, cv::format("FPS = %.2f",fps), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 4);
        img_result = frame.clone();

    }

}

void FaceRecognition::process(cv::Mat &frame)
{

    int frameHeight = frame.rows;
    int frameWidth = frame.cols;

    cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300.0, 300.0), cv::Scalar(104.0, 177.0, 123.0), true, false);
    net.setInput(inputBlob, "data");
    cv::Mat detection = net.forward("detection_out");
    
    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    for(int i = 0; i < detectionMat.rows; i++)
    {
        float confidence = detectionMat.at<float>(i, 2);

        if(confidence > 0.7)
        {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);

            cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0),2, 4);
        }
    }
}
