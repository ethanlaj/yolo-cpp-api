#include "yolov7.h"
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/dnn/dnn.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <thread>


// Function declarations
void process_webcam(YOLOv7* yolov7, const std::vector<std::string>& classNames);

std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}

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
    
    process_webcam(&yolov7, classNames);

    return 0;
}

void process_webcam(YOLOv7* yolov7, const std::vector<std::string>& classNames) {
	int capture_width = 1280 ;
    int capture_height = 720 ;
    int display_width = 1280 ;
    int display_height = 720 ;
    int framerate = 30 ;
    int flip_method = 0 ;

    std::string pipeline = gstreamer_pipeline(capture_width,
	capture_height,
	display_width,
	display_height,
	framerate,
	flip_method);
	
	cv::VideoCapture cap(pipeline, cv::CAP_GSTREAMER);

    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open the webcam." << std::endl;
        return;
    }
    
	// Add a delay
	std::this_thread::sleep_for(std::chrono::seconds(1));

    cv::Mat frame;

    while (true) {
		if (!cap.read(frame)) {
			std::cerr << "Error: Unable to capture a frame from the camera." << std::endl;
			continue;
		}
		
        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            break;
        }
        
        cv::flip(frame, frame, 0); // Flip the frame vertically
        
        cv::Mat resized_frame;
		cv::resize(frame, resized_frame, cv::Size(416, 416));
        		
		int64 start_ticks = cv::getTickCount();
        std::vector<Detection> detections = yolov7_detect(yolov7, resized_frame);

        for (const auto& detection : detections) {
            int class_id = detection.class_id;
            float confidence = detection.confidence;

            cv::Rect bbox(
                detection.bbox.x * frame.cols / resized_frame.cols,
                detection.bbox.y * frame.rows / resized_frame.rows,
                detection.bbox.width * frame.cols / resized_frame.cols,
                detection.bbox.height * frame.rows / resized_frame.rows
            );

            std::string label = classNames[class_id] + ": " + cv::format("%.2f", confidence);
            int baseLine;
            cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
            cv::putText(frame, label, cv::Point(bbox.x, bbox.y - labelSize.height),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
        }
        
        int64 end_ticks = cv::getTickCount();
		double elapsed_time_ms = (end_ticks - start_ticks) * 1000 / cv::getTickFrequency();
		std::cout << "Elapsed time: " << elapsed_time_ms << " ms" << std::endl;

        cv::imshow("YOLOv7 Object Detection", frame);

        int key = cv::waitKey(1) & 0xFF;
        if (key == 27 || key == 'q') { // Exit if the user presses 'ESC' or 'q'
            break;
        }
    }
}

