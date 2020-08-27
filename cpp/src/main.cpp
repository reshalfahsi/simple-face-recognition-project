#include "FaceRecognition/FaceRecognition.h"
#include <signal.h>

using namespace face_recognition;

int main(int argc, char *argv[]) {
  
    std::cout << "Simple Face Recognition Project by Resha Al-Fahsi" << '\n';

    FaceRecognition facerec;

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
