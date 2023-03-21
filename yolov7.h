#ifndef YOLOV7_H
#define YOLOV7_H

#include <opencv2/core.hpp>
#include <opencv2/dnn.hpp>

typedef struct YOLOv7 {
    cv::dnn::Net net;
    float confThreshold;
    float nmsThreshold;
    int inputWidth;
    int inputHeight;
} YOLOv7;

struct Detection {
    int class_id;
    float confidence;
    cv::Rect bbox;
};

YOLOv7 yolov7_init(const std::string& modelPath, const std::string& configPath,
                   float confThreshold, float nmsThreshold, int inputWidth, int inputHeight);

void yolov7_free(YOLOv7* yolov7);

std::vector<Detection> yolov7_detect(YOLOv7* yolov7, const cv::Mat& inputImage);

#endif // YOLOV7_H
