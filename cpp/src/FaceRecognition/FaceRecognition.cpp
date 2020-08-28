#include "FaceRecognition/FaceRecognition.h"

using namespace face_recognition;

FaceRecognition::FaceRecognition(std::string weight, std::string config, std::string recog, std::string dataset, std::string webcam){

    img_result = cv::Mat::zeros(cv::Size(640, 480), CV_64FC1);

    std::string weight_file = "../model/opencv_face_detector_uint8.pb";
    std::string config_file = "../model/opencv_face_detector.pbtxt";
    std::string recog_file = "../model/openface.nn4.small2.v1.t7";
    device = "/dev/video0";
    dataset_dir = "../../dataset";

    if (weight != "") weight_file = weight;
    if (config != "") config_file = config;
    if (webcam != "") device = webcam;
    if (recog != "") recog_file = recog;
    if (dataset != "") dataset_dir = dataset;

    net = cv::dnn::readNetFromTensorflow(weight_file, config_file);
    net_recog = cv::dnn::readNetFromTorch(recog_file);


    dataset_preprocessing();

}

FaceRecognition::~FaceRecognition(){


}


void FaceRecognition::dataset_preprocessing(){

    std::vector<std::string> filenames;    

    std::string dir = "";
    int found = dataset_dir.find_last_of("/\\");
    int end = dataset_dir.size()-1;
    if(found == end) dir = std::string(dataset_dir) + std::string("*.jpg");
    else dir = std::string(dataset_dir) + std::string("/*.jpg");

    cv::glob(dir, filenames);

    for (auto name : filenames)
    {
        
        cv::Mat im = cv::imread(name);
        
        cv::Mat inputBlobRecog = cv::dnn::blobFromImage(im, 1.0/255.0, cv::Size(96.0, 96.0), cv::Scalar(0, 0, 0, 0), true, false);
        net_recog.setInput(inputBlobRecog);
        cv::Mat recognition = net_recog.forward();

        std::size_t found = name.find_last_of("/\\");
        name = name.substr(found+1);
        found = name.find_last_of(".\\");
        name = name.substr(0, found);
        
        name_list.push_back(name);
        face_encoding[name] = recognition.clone();

    }

}

std::string FaceRecognition::compare_face(cv::Mat encoding){

    std::string who_am_i = "Unknown";
    auto best_score = 0.6;
 
    for(auto name : name_list){

        double score = 0.0;
        double mag_known = 0.0;
        double mag_new = 0.0;

        for(int col = 0; col < face_encoding[name].cols; col++){
	    auto val_known = face_encoding[name].at<float>(0, col);
            auto val_new = encoding.at<float>(0, col);
            mag_known += std::pow(val_known, 2.0);
            mag_new += std::pow(val_new, 2.0);
        }

        mag_known = std::sqrt(mag_known);
        mag_new = std::sqrt(mag_new);        

        for(int col = 0; col < encoding.cols; col++){
            
            auto val_known = face_encoding[name].at<float>(0, col);
            auto val_new = encoding.at<float>(0, col);
            score += val_known * val_new; 

        }

        score /= (mag_known * mag_new);
        
        if(score > best_score){
            
           best_score = score;
           who_am_i = name;

        }

    }

    return who_am_i;

}

void FaceRecognition::start(){

    run = true;
    thread = std::thread(&FaceRecognition::loop, this);

}

void FaceRecognition::loop(){

    cv::VideoCapture cap(device);
    if (!cap.isOpened())
    {
        run = false;
	std::cout << "Cannot open the video cam" << std::endl;
	return;
    }
    
    double dWidth = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    double dHeight = cap.get(cv::CAP_PROP_FRAME_HEIGHT);

    std::cout << "Frame Size: " << dWidth << "x" << dHeight << std::endl;

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

    std::string name = "Unknown";

    cv::Mat inputBlob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300.0, 300.0), cv::Scalar(104.0, 177.0, 123.0), true, false);
    net.setInput(inputBlob, "data");
    cv::Mat detection = net.forward("detection_out");

    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    cv::Mat inputBlobRecog = cv::dnn::blobFromImage(frame, 1.0/255.0, cv::Size(96.0, 96.0), cv::Scalar(0, 0, 0, 0), true, false);
    net_recog.setInput(inputBlobRecog);
    cv::Mat recognition = net_recog.forward();

    name = compare_face(recognition);

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
            cv::rectangle(frame, cv::Point(x1, y2 - 35), cv::Point(x2, y2), cv::Scalar(0, 255, 0), cv::FILLED);
            auto font = cv::FONT_HERSHEY_DUPLEX;
            cv::putText(frame, name, cv::Point(x1 + 6, y2 - 6), font, 1.0, cv::Scalar(255, 255, 255), 1);
        }
    }
}
