#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include "VideoWidget.h"
#include <QPushButton>

// Add this line
namespace Ui {
    class MainWindow;
}

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

public slots:
    void startDetection();
    void stopDetection();

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

    // Add this line
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
