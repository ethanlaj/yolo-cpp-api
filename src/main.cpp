#include <iostream>
#include <chrono>
#include <thread>

#include <QApplication>
#include <QCoreApplication>
#include <QThread>
#include <QTimer>
#include <QObject>

#include "gui/MainWindow.h"
#include "gui/TimerHandler.h"
#include "webcam.h"
#include "yolo.h"

int main(int argc, char** argv) {
	QApplication app(argc, argv);
	
    MainWindow mainWindow;
    mainWindow.setFixedSize(1280, 720); // Set a fixed size for the main window
    mainWindow.videoProcessingThread = nullptr;

    mainWindow.show();

    std::string model_path = "models/weights/yolov7-tiny.weights";
    std::string config_path = "models/cfg/yolov7-tiny.cfg";
    std::string classNames_path = "models/names/coco.names";

    YOLO yolo = yolo_init(model_path, config_path, 0.25, 0.5, 416, 416);
    std::vector<std::string> classNames = readClassNames(classNames_path);
    
    QObject::connect(&mainWindow, &MainWindow::confThresholdChanged, [&yolo](float value) {
        yolo.confThreshold = value;
    });

    QObject::connect(&mainWindow, &MainWindow::nmsThresholdChanged, [&yolo](float value) {
        yolo.nmsThreshold = value;
    });

    int capture_width = 1280;
    int capture_height = 720;
    int display_width = 1280;
    int display_height = 720;
    int framerate = 30;
    int flip_method = 0;
    std::string pipeline = gstreamer_pipeline(capture_width, capture_height, display_width, display_height, framerate, flip_method);
	mainWindow.cap.open(pipeline, cv::CAP_GSTREAMER);

    QObject::connect(&mainWindow, &MainWindow::startClicked, [&]() {
        if (mainWindow.videoProcessingThread && mainWindow.videoProcessingThread->isRunning()) {
            mainWindow.videoProcessingThread->requestInterruption();
            mainWindow.videoProcessingThread->wait(0);
        }
    
        mainWindow.videoProcessingThread = new QThread();
        TimerHandler *timerHandler = new TimerHandler();
        timerHandler->moveToThread(mainWindow.videoProcessingThread);
        mainWindow.videoProcessingThread->start();
        
        QObject::connect(mainWindow.videoProcessingThread, &QThread::finished, mainWindow.videoProcessingThread, &QThread::deleteLater);

		QObject::connect(mainWindow.videoProcessingThread, &QThread::started, timerHandler, [&]() {
			QObject::connect(timerHandler->timer, &QTimer::timeout, [&]() {
				try {
					cv::Mat frame;
					if (!mainWindow.cap.read(frame)) {
						std::cerr << "Error: Unable to capture a frame from the camera." << std::endl;
						return;
					}

					if (frame.empty()) {
						std::cerr << "Error: Empty frame captured." << std::endl;
						return;
					}

					cv::flip(frame, frame, 0); // Flip the frame vertically

					cv::Mat resized_frame;
					cv::resize(frame, resized_frame, cv::Size(416, 416));

					std::vector<Detection> detections = yolo_detect(&yolo, resized_frame);

					for (const auto& detection : detections) {
						int class_id = detection.class_id;
						float confidence = detection.confidence;

						// Scale the bounding box back to the original frame size
						cv::Rect scaled_bbox(
							detection.bbox.x * frame.cols / resized_frame.cols,
							detection.bbox.y * frame.rows / resized_frame.rows,
							detection.bbox.width * frame.cols / resized_frame.cols,
							detection.bbox.height * frame.rows / resized_frame.rows
						);

						cv::rectangle(frame, scaled_bbox, cv::Scalar(0, 255, 0), 2);

						std::string label = classNames[class_id] + ": " + cv::format("%.2f", confidence);
						int baseLine;
						cv::Size labelSize = cv::getTextSize(label, cv::FONT_HERSHEY_SIMPLEX, 0.5, 1, &baseLine);
						cv::rectangle(frame, cv::Rect(cv::Point(scaled_bbox.x, scaled_bbox.y - labelSize.height),
										  cv::Size(labelSize.width, labelSize.height + baseLine)),
									  cv::Scalar(0, 255, 0), cv::FILLED);
						cv::putText(frame, label, cv::Point(scaled_bbox.x, scaled_bbox.y), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 0), 1);
					}

				QImage img = QImage((const unsigned char*) (frame.data), frame.cols, frame.rows, frame.step, QImage::Format_RGB888).rgbSwapped();
				mainWindow.showFrame(img);
				
				} catch (const cv::Exception &e) {
					std::cerr << "OpenCV error: " << e.what() << std::endl;
					mainWindow.stopVideoProcessing();
				} catch (const std::exception &e) {
					std::cerr << "Error: " << e.what() << std::endl;
					mainWindow.stopVideoProcessing();
				}
			});
			timerHandler->startTimer(1000 / framerate);
        });
		
        QObject::connect(mainWindow.videoProcessingThread, &QThread::finished, timerHandler, &TimerHandler::deleteLater);
        QObject::connect(&mainWindow, &MainWindow::stopClicked, timerHandler, &TimerHandler::stopTimer);
        QObject::connect(&mainWindow, &MainWindow::stopClicked, [&]() {
			if (mainWindow.videoProcessingThread && mainWindow.videoProcessingThread->isRunning()) {
				mainWindow.videoProcessingThread->requestInterruption();
				mainWindow.videoProcessingThread->quit();
				mainWindow.videoProcessingThread->wait(0);
			}
        });
    });

    int ret = app.exec();

    return ret;
}

