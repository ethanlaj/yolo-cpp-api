#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSlider>
#include <QPointer>
#include <QPushButton>
#include <opencv2/opencv.hpp>
#include <opencv2/videoio/videoio_c.h>
#include <opencv2/dnn/dnn.hpp>

#include "VideoWidget.h"
#include "../yolo.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    cv::VideoCapture cap;
    QPointer<QThread> videoProcessingThread;
    
    YOLO *yolo;
    std::vector<std::string> *classNames;

    void connectSignalsSlots();
    void showFrame(const QImage &frame);
    void showFPS(double fps);
    void stopVideoProcessing();
    
protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void confThresholdChanged(float value);
    void nmsThresholdChanged(float value);
    void startClicked();
    void stopClicked();

private slots:
    void onConfThresholdSliderValueChanged(int value);
    void onNmsThresholdSliderValueChanged(int value);
    void onStartButtonClicked();
    void onStopButtonClicked();

private:
    VideoWidget *videoWidget;
    QSlider *confThresholdSlider;
    QSlider *nmsThresholdSlider;
    QPushButton *startButton;
    QPushButton *stopButton;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
