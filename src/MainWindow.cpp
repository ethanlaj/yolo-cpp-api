#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), videoWidget(new VideoWidget(this)) {
    setCentralWidget(videoWidget);
}

MainWindow::~MainWindow() {
    delete videoWidget;
}

void MainWindow::showFrame(const QImage &frame) {
    videoWidget->showFrame(frame);
}
