#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <thread>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

namespace face_recognition {

class FaceRecognition{

public:
    FaceRecognition();
    ~FaceRecognition();

    void start();
    void stop(){
        
        run = false;
 
	if(thread.joinable()){
            thread.join();
        }
    }

    void loop();
    bool ok() { return run; }

    cv::Mat result() { return img_result; }

    void process(cv::Mat &frame);
    
private:
    std::thread thread;
    bool run;
    cv::Mat img_result;
    cv::dnn::Net net;

};

}

#endif // FACERECOGNITION_H
