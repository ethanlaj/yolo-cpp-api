#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include "VideoWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showFrame(const QImage &frame);

signals:
    void confThresholdChanged(float value);
    void nmsThresholdChanged(float value);
    void startClicked();
    void stopClicked();

private slots:
    void onConfThresholdSliderValueChanged(int value);
    void onNmsThresholdSliderValueChanged(int value);

private:
    VideoWidget *videoWidget;
    QSlider *confThresholdSlider;
    QSlider *nmsThresholdSlider;
};

#endif // MAINWINDOW_H
