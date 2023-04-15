#include <fstream>
#include <iostream>
#include <opencv2/opencv.hpp>

#include "yolo.h"

void yolo_init(YOLO* yolo, const std::string& modelPath, const std::string& configPath,
                   float confThreshold, float nmsThreshold, int inputWidth, int inputHeight) {
    yolo->net = cv::dnn::readNetFromDarknet(configPath, modelPath);
    yolo->confThreshold = confThreshold;
    yolo->nmsThreshold = nmsThreshold;
    yolo->inputWidth = inputWidth;
    yolo->inputHeight = inputHeight;

    // Set preferable backend and target to CUDA if available
    if (cv::cuda::getCudaEnabledDeviceCount() > 0) {
        yolo->net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        yolo->net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        std::cout << "Using GPU acceleration with CUDA." << std::endl;
    } else {
        std::cout << "Using CPU for computation. No compatible GPU found or OpenCV not built with CUDA support." << std::endl;
    }
}

std::vector<Detection> yolo_detect(YOLO* yolo, const cv::Mat& inputImage) {
    std::vector<Detection> nmsDetections;
    
    cv::Mat blob;
    cv::dnn::blobFromImage(inputImage, blob, 1 / 255.0, cv::Size(yolo->inputWidth, yolo->inputHeight), cv::Scalar(), true, false);
    yolo->net.setInput(blob);

    std::vector<cv::Mat> outputLayers;
    yolo->net.forward(outputLayers, yolo->net.getUnconnectedOutLayersNames());

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
            if (confidence > yolo->confThreshold) {
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
    cv::dnn::NMSBoxes(boxes, confidences, yolo->confThreshold, yolo->nmsThreshold, indices);

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
