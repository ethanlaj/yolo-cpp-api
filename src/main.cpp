#include <iostream>

#include <QApplication>

#include "gui/MainWindow.h"
#include "camera.h"
#include "yolo.h"

int main(int argc, char** argv) {
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setFixedSize(1280, 720);
    mainWindow.videoProcessingThread = nullptr;

    mainWindow.show();

    std::string model_path = "models/weights/yolov7-tiny.weights";
    std::string config_path = "models/cfg/yolov7-tiny.cfg";
    std::string classNames_path = "models/names/coco.names";

    YOLO yolo = yolo_init(model_path, config_path, 0.25, 0.5, 416, 416);
    std::vector<std::string> classNames = readClassNames(classNames_path);

    int capture_width = 1280;
    int capture_height = 720;
    int display_width = 1280;
    int display_height = 720;
    int framerate = 30;
    int flip_method = 0;

    setupCamera(mainWindow, capture_width, capture_height, display_width, display_height, framerate, flip_method);

    mainWindow.connectSignalsSlots(yolo, classNames, framerate);

    int ret = app.exec();

    return ret;
}
