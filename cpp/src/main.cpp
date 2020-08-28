#include "FaceRecognition/FaceRecognition.h"
#include <signal.h>

using namespace face_recognition;

int main(int argc, char **argv) {
  
    std::cout << "Simple Face Recognition Project by Resha Al-Fahsi" << '\n';

    auto weight = "../model/opencv_face_detector_uint8.pb";
    auto config = "../model/opencv_face_detector.pbtxt";
    auto webcam = "/dev/video0";

    for(int idx = 0; idx < argc; idx++){

        std::cout << argv[idx] << '\n';

        if(argv[idx] == "--cam"){
            if(idx+1<argc){
                webcam = argv[idx+1];
            }
        }

        if((argv[idx] == "--weight") || (argv[idx] == "-w")){
            if(idx+1<argc){
                weight = argv[idx+1];
            }
        }

        if((argv[idx] == "--config")){
            if(idx+1<argc){
                config = argv[idx+1];
            }
        }

    }

    FaceRecognition facerec(weight, config, webcam);

    facerec.start();

    while(facerec.ok()){
	
        try{
            auto img = facerec.result();
            if((img.rows>0) && (img.cols>0)){
                cv::namedWindow("Simple Face Recognition Project");
                cv::imshow( "Simple Face Recognition Project", img );
            }
            if( cv::waitKey(10) == 27 ) break; 
        }       
	catch(int e){
            std::cout << "An exception occurred. Exception Nr. " << e << '\n';
        }
    }

    facerec.stop();
    
    return 0;
}
