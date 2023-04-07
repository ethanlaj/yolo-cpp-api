#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "VideoWidget.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void showFrame(const QImage &frame);

private:
    VideoWidget *videoWidget;
};

#endif // MAINWINDOW_H
