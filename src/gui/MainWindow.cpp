#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

#include "MainWindow.h"
#include "TimerHandler.h"
#include "ui_MainWindow.h"
#include "../camera.h"
#include "../yolo.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {

    ui->setupUi(this);

    videoWidget = new VideoWidget(ui->videoWidgetContainer);
    videoWidget->setGeometry(ui->videoWidgetContainer->rect());
		
	QSlider *confSlider = ui->confThresholdSlider;
	QLabel *confValueLabel = ui->confValueLabel;
	confValueLabel->setText(QString("%1").arg(static_cast<float>(confSlider->value()) / 100, 0, 'f', 2));
	connect(confSlider, &QSlider::valueChanged, this, &MainWindow::onConfThresholdSliderValueChanged);
	
    QSlider *nmsSlider = ui->nmsThresholdSlider;	
	QLabel *nmsValueLabel = ui->nmsValueLabel;
	nmsValueLabel->setText(QString("%1").arg(static_cast<float>(nmsSlider->value()) / 100, 0, 'f', 2));
    connect(nmsSlider, &QSlider::valueChanged, this, &MainWindow::onNmsThresholdSliderValueChanged);

    startButton = ui->startButton;
    connect(startButton, &QPushButton::clicked, this, &MainWindow::onStartButtonClicked);
    
    stopButton = ui->stopButton;
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::onStopButtonClicked);
}

MainWindow::~MainWindow() {	 
	if (cap.isOpened()) {
        cap.release();
    }
    
    if (videoProcessingThread) {
        videoProcessingThread->requestInterruption();
        videoProcessingThread->quit();
        videoProcessingThread->wait(static_cast<unsigned long>(-1));
    }
    
    delete videoWidget;
    delete ui;
}

void MainWindow::connectSignalsSlots(YOLO &yolo, const std::vector<std::string> &classNames, int framerate) {
    QObject::connect(this, &MainWindow::confThresholdChanged, [&yolo](float value) {
        yolo.confThreshold = value;
    });

    QObject::connect(this, &MainWindow::nmsThresholdChanged, [&yolo](float value) {
        yolo.nmsThreshold = value;
    });

    QObject::connect(this, &MainWindow::startClicked, [&]() {
        if (videoProcessingThread && videoProcessingThread->isRunning()) {
            videoProcessingThread->requestInterruption();
            videoProcessingThread->wait(0);
        }

        videoProcessingThread = new QThread();
        TimerHandler *timerHandler = new TimerHandler();
        timerHandler->moveToThread(videoProcessingThread);
        videoProcessingThread->start();

        QObject::connect(videoProcessingThread, &QThread::finished, videoProcessingThread, &QThread::deleteLater);

        QObject::connect(videoProcessingThread, &QThread::started, timerHandler, [&]() {
			QObject::connect(timerHandler->timer, &QTimer::timeout, [&]() {
				processFrame(*this, cap, yolo, classNames);
			});
			timerHandler->startTimer(1000 / framerate);
		});

        QObject::connect(videoProcessingThread, &QThread::finished, timerHandler, &TimerHandler::deleteLater);
        QObject::connect(this, &MainWindow::stopClicked, timerHandler, &TimerHandler::stopTimer);
        QObject::connect(this, &MainWindow::stopClicked, [&]() {
            if (videoProcessingThread && videoProcessingThread->isRunning()) {
                videoProcessingThread->requestInterruption();
                videoProcessingThread->quit();
                videoProcessingThread->wait(0);
            }
        });
    });
}

void MainWindow::showFrame(const QImage &frame) {
    videoWidget->showFrame(frame);
}

void MainWindow::onConfThresholdSliderValueChanged(int value) {
   float confThreshold = static_cast<float>(value) / 100;
   emit confThresholdChanged(confThreshold);
   ui->confValueLabel->setText(QString("%1").arg(confThreshold, 0, 'f', 2));
}

void MainWindow::onNmsThresholdSliderValueChanged(int value) {
   float nmsThreshold = static_cast<float>(value) / 100;
   emit nmsThresholdChanged(nmsThreshold);
   ui->nmsValueLabel->setText(QString("%1").arg(nmsThreshold, 0, 'f', 2));
}

void MainWindow::startDetection() {
    emit startClicked();
}

void MainWindow::stopDetection() {
    emit stopClicked();
}

void MainWindow::onStartButtonClicked() {
    startDetection();
}

void MainWindow::onStopButtonClicked() {	
    stopDetection();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    stopVideoProcessing();
    QMainWindow::closeEvent(event);
}

void MainWindow::stopVideoProcessing() {
    if (videoProcessingThread && videoProcessingThread->isRunning()) {
        emit stopClicked();
    }
}
