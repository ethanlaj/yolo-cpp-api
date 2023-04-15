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

    mainWindow.connectSignalsSlots();

    int ret = app.exec();

    return ret;
}
