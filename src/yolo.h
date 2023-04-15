#ifndef YOLO_H
#define YOLO_H

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

struct YOLO {
    cv::dnn::Net net;
    float confThreshold;
    float nmsThreshold;
    int inputWidth;
    int inputHeight;

    ~YOLO() {
		std::cout << "YOLO deconstructor called" << std::endl;
        //net.~Net();
    }
};

struct Detection {
    int class_id;
    float confidence;
    cv::Rect bbox;
};

void yolo_init(YOLO* yolo, const std::string& modelPath, const std::string& configPath,
                   float confThreshold, float nmsThreshold, int inputWidth, int inputHeight);

//void yolov7_free(YOLOv7* yolov7);

std::vector<Detection> yolo_detect(YOLO* yolo, const cv::Mat& inputImage);

std::vector<std::string> readClassNames(const std::string& path);

#endif // YOLO_H
