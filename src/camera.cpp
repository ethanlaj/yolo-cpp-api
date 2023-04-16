#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/dnn/dnn.hpp>

#include "camera.h"

std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method) {
    return "nvarguscamerasrc ! video/x-raw(memory:NVMM), width=(int)" + std::to_string(capture_width) + ", height=(int)" +
           std::to_string(capture_height) + ", framerate=(fraction)" + std::to_string(framerate) +
           "/1 ! nvvidconv flip-method=" + std::to_string(flip_method) + " ! video/x-raw, width=(int)" + std::to_string(display_width) + ", height=(int)" +
           std::to_string(display_height) + ", format=(string)BGRx ! videoconvert ! video/x-raw, format=(string)BGR ! appsink";
}


void setupCamera(MainWindow &mainWindow, int framerate) {
	int capture_width = 1280;
	int capture_height = 720;
	int display_width = 1280;
	int display_height = 720;
	int flip_method = 0;
	
    std::string pipeline = gstreamer_pipeline(capture_width, capture_height, display_width, display_height, framerate, flip_method);
    mainWindow.cap.open(pipeline, cv::CAP_GSTREAMER);
}

void processFrame(MainWindow &mainWindow, cv::VideoCapture &cap, YOLO &yolo, const std::vector<std::string> &classNames) {
    try {
        cv::Mat frame;
        if (!cap.read(frame)) {
            std::cerr << "Error: Unable to capture a frame from the camera." << std::endl;
            return;
        }

        if (frame.empty()) {
            std::cerr << "Error: Empty frame captured." << std::endl;
            return;
        }

        cv::flip(frame, frame, 0); // Flip the frame vertically

        cv::Mat resized_frame;
        cv::resize(frame, resized_frame, cv::Size(416, 416));

        std::vector<Detection> detections = yolo_detect(&yolo, resized_frame);
        addDetectionsToFrame(frame, resized_frame, detections, classNames);

        updateFPS(mainWindow);

        QImage img = QImage((const unsigned char*) (frame.data), frame.cols, frame.rows, frame.step, QImage::Format_RGB888).rgbSwapped();
        mainWindow.showFrame(img);
    } catch (const cv::Exception &e) {
        std::cerr << "OpenCV error: " << e.what() << std::endl;
        mainWindow.stopVideoProcessing();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        mainWindow.stopVideoProcessing();
    }
}

void updateFPS(MainWindow &mainWindow) {
	static int frame_counter = 0;
    static double last_fps_calculation_time = 0.0;
    static double fps = 0.0;
    
	frame_counter++;
	double current_time = static_cast<double>(cv::getTickCount()) / cv::getTickFrequency();
	if (current_time - last_fps_calculation_time >= 1.0) {
		fps = frame_counter / (current_time - last_fps_calculation_time);
		last_fps_calculation_time = current_time;
		frame_counter = 0;
	}
	
	// Display FPS on the GUI
	mainWindow.showFPS(fps);
}

void addDetectionsToFrame(cv::Mat &frame, cv::Mat &resized_frame, const std::vector<Detection> &detections, const std::vector<std::string> &classNames) {
    for (const auto& detection : detections) {
        int class_id = detection.class_id;
        float confidence = detection.confidence;

        // Scale the bounding box back to the original frame size
        cv::Rect scaled_bbox(
            detection.bbox.x * frame.cols / resized_frame.cols,
            detection.bbox.y * frame.rows / resized_frame.rows,
            detection.bbox.width * frame.cols / resized_frame.cols,
            detection.bbox.height * frame.rows / resized_frame.rows
        );

        cv::rectangle(frame, scaled_bbox, cv::Scalar(0, 255, 0), 2);

        std::string label = classNames[class_id] + ": " + cv::format("%.2f", confidence);
        int baseLine;
        cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
        cv::rectangle(frame, cv::Rect(cv::Point(scaled_bbox.x, scaled_bbox.y - labelSize.height),
                              cv::Size(labelSize.width, labelSize.height + baseLine)),
                      cv::Scalar(0, 255, 0), cv::FILLED);
        cv::putText(frame, label, cv::Point(scaled_bbox.x, scaled_bbox.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
    }
}


