#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <thread>

#include <opencv2/opencv.hpp>

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

private:
    std::thread thread;
    bool run;
    cv::Mat img_result;

};

}

#endif // FACERECOGNITION_H
