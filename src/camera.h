#ifndef CAMERA_H
#define CAMERA_H

#include "yolo.h"
#include "gui/MainWindow.h"

std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method);

void setupCamera(MainWindow &mainWindow, int framerate);

void processFrame(MainWindow &mainWindow, cv::VideoCapture &cap, YOLO &yolo, const std::vector<std::string> &classNames);

void addDetectionsToFrame(cv::Mat &frame, cv::Mat &resized_frame, const std::vector<Detection> &detections, const std::vector<std::string> &classNames);

#endif //CAMERA_H
