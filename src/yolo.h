#ifndef YOLO_H
#define YOLO_H

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

typedef struct {
    cv::dnn::Net net;
    float confThreshold;
    float nmsThreshold;
    int inputWidth;
    int inputHeight;
} YOLO;

struct Detection {
    int class_id;
    float confidence;
    cv::Rect bbox;
};

YOLO yolo_init(const std::string& modelPath, const std::string& configPath,
                   float confThreshold, float nmsThreshold, int inputWidth, int inputHeight);

//void yolov7_free(YOLOv7* yolov7);

std::vector<Detection> yolo_detect(YOLO* yolo, const cv::Mat& inputImage);

std::vector<std::string> readClassNames(const std::string& path);

#endif // YOLO_H
