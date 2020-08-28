#ifndef FACERECOGNITION_H
#define FACERECOGNITION_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <thread>
#include <map>
#include <cmath>

#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>

namespace face_recognition {

class FaceRecognition{

typedef std::map<std::string, cv::Mat> Encoding;

public:
    FaceRecognition(std::string weight, std::string config, std::string recog, std::string dataset, std::string webcam);
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

    void dataset_preprocessing();
  
    std::string compare_face(cv::Mat encoding);

    void process(cv::Mat &frame);
    
private:
    std::thread thread;
    bool run;
    cv::Mat img_result;
    cv::dnn::Net net;
    cv::dnn::Net net_recog;
    std::string device;
    std::vector<std::string> name_list;
    Encoding face_encoding;
    std::string dataset_dir;

};

}

#endif // FACERECOGNITION_H
