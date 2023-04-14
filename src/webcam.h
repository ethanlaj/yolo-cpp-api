#ifndef WEBCAM_H
#define WEBCAM_H

#include "yolo.h"

//void process_webcam(YOLO* yolo, const std::vector<std::string>& classNames);
std::string gstreamer_pipeline (int capture_width, int capture_height, int display_width, int display_height, int framerate, int flip_method);

#endif //WEBCAM_H
