#include "yolov7.h"
#include <opencv2/opencv.hpp>
#include <opencv2/dnn/dnn.hpp>
#include <fstream>
#include <iostream>

YOLOv7 yolov7_init(const std::string& modelPath, const std::string& configPath,
                   float confThreshold, float nmsThreshold, int inputWidth, int inputHeight) {
    YOLOv7 yolov7;
    yolov7.net = cv::dnn::readNetFromDarknet(configPath, modelPath);
    yolov7.confThreshold = confThreshold;
    yolov7.nmsThreshold = nmsThreshold;
    yolov7.inputWidth = inputWidth;
    yolov7.inputHeight = inputHeight;

    // Set preferable backend and target to CUDA if available
    if (cv::cuda::getCudaEnabledDeviceCount() > 0) {
        yolov7.net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        yolov7.net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        std::cout << "Using GPU acceleration with CUDA." << std::endl;
    } else {
        std::cout << "Using CPU for computation. No compatible GPU found or OpenCV not built with CUDA support." << std::endl;
    }

    return yolov7;
}

std::vector<Detection> yolov7_detect(YOLOv7* yolov7, const cv::Mat& inputImage) {
    std::vector<Detection> nmsDetections;
    
    cv::Mat blob;
    cv::dnn::blobFromImage(inputImage, blob, 1 / 255.0, cv::Size(yolov7->inputWidth, yolov7->inputHeight), cv::Scalar(), true, false);
    yolov7->net.setInput(blob);

    std::vector<cv::Mat> outputLayers;
    yolov7->net.forward(outputLayers, yolov7->net.getUnconnectedOutLayersNames());

    std::vector<int> classIds;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (const auto &outputLayer : outputLayers) {
        auto* data = (float*)outputLayer.data;
        for (int j = 0; j < outputLayer.rows; ++j, data += outputLayer.cols) {
            cv::Mat scores = outputLayer.row(j).colRange(5, outputLayer.cols);
            cv::Point classIdPoint;
            double confidence;
            cv::minMaxLoc(scores, nullptr, &confidence, nullptr, &classIdPoint);
            if (confidence > yolov7->confThreshold) {
                int centerX = static_cast<int>(data[0] * inputImage.cols);
                int centerY = static_cast<int>(data[1] * inputImage.rows);
                int width = static_cast<int>(data[2] * inputImage.cols);
                int height = static_cast<int>(data[3] * inputImage.rows);
                int left = centerX - width / 2;
                int top = centerY - height / 2;

                classIds.push_back(classIdPoint.x);
                confidences.push_back(static_cast<float>(confidence));
                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
    }

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, yolov7->confThreshold, yolov7->nmsThreshold, indices);

    for (int idx : indices) {
        Detection detection;
        detection.class_id = classIds[idx];
        detection.confidence = confidences[idx];
        detection.bbox = boxes[idx];
        nmsDetections.push_back(detection);
    }

    return nmsDetections;
}

std::vector<std::string> readClassNames(const std::string& classNamesPath) {
    std::vector<std::string> classNames;
    std::ifstream classNamesFile(classNamesPath);
    if (classNamesFile.is_open()) {
        std::string className;
        while (std::getline(classNamesFile, className)) {
            classNames.push_back(className);
        }
        classNamesFile.close();
    }
    return classNames;
}

int main(int argc, char** argv) {
    if (argc != 6) {
        printf("Usage: %s <image_path> <model_path> <config_path> <classNames_path> <output_image_path>\n", argv[0]);
        return 1;
    }

    std::string image_path = argv[1];
    std::string model_path = argv[2];
    std::string config_path = argv[3];
    std::string classNames_path = argv[4];
    std::string output_image_path = argv[5];

    cv::Mat input_image = cv::imread(image_path, cv::IMREAD_COLOR);

    YOLOv7 yolov7 = yolov7_init(model_path, config_path, 0.25, 0.5, 640, 640);
    
    std::vector<std::string> classNames = readClassNames(classNames_path);
    std::vector<Detection> detections = yolov7_detect(&yolov7, input_image);

    for (const auto& detection : detections) {
        int class_id = detection.class_id;
        float confidence = detection.confidence;
        cv::Rect bbox = detection.bbox;

        // Draw the bounding box
        cv::rectangle(input_image, bbox, cv::Scalar(0, 255, 0), 2);

        // Display class name and confidence
        std::string label = classNames[class_id] + ": " + cv::format("%.2f", confidence);
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        cv::putText(input_image, label, cv::Point(bbox.x, bbox.y - labelSize.height),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    }

    cv::imwrite(output_image_path, input_image);

    return 0;
}
