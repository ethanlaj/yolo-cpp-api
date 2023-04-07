#include "MainWindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), videoWidget(new VideoWidget(this)) {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    QHBoxLayout *topLayout = new QHBoxLayout();

    QVBoxLayout *sliderLayout = new QVBoxLayout();
    QLabel *confThresholdLabel = new QLabel("Confidence Threshold", this);
    sliderLayout->addWidget(confThresholdLabel);

    QHBoxLayout *confSliderLayout = new QHBoxLayout();
    QLabel *confMinLabel = new QLabel("0", this);
    QLabel *confMaxLabel = new QLabel("1", this);
    confThresholdSlider = new QSlider(Qt::Horizontal, this);
    confThresholdSlider->setMinimum(0);
    confThresholdSlider->setMaximum(100);
    confThresholdSlider->setValue(25);
    QLabel *confValueLabel = new QLabel("0.25", this);
    connect(confThresholdSlider, &QSlider::valueChanged, this, [confValueLabel](int value) {
        confValueLabel->setText(QString::number(static_cast<float>(value) / 100, 'f', 2));
    });
    connect(confThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(onConfThresholdSliderValueChanged(int)));

    confSliderLayout->addWidget(confMinLabel);
    confSliderLayout->addWidget(confThresholdSlider);
    confSliderLayout->addWidget(confMaxLabel);
    sliderLayout->addLayout(confSliderLayout);
    sliderLayout->addWidget(confValueLabel);

    QLabel *nmsThresholdLabel = new QLabel("NMS Threshold", this);
    sliderLayout->addWidget(nmsThresholdLabel);

    QHBoxLayout *nmsSliderLayout = new QHBoxLayout();
    QLabel *nmsMinLabel = new QLabel("0", this);
    QLabel *nmsMaxLabel = new QLabel("1", this);
    nmsThresholdSlider = new QSlider(Qt::Horizontal, this);
    nmsThresholdSlider->setMinimum(0);
    nmsThresholdSlider->setMaximum(100);
    nmsThresholdSlider->setValue(50);
    QLabel *nmsValueLabel = new QLabel("0.50", this);
    connect(nmsThresholdSlider, &QSlider::valueChanged, this, [nmsValueLabel](int value) {
        nmsValueLabel->setText(QString::number(static_cast<float>(value) / 100, 'f', 2));
    });
    connect(nmsThresholdSlider, SIGNAL(valueChanged(int)), this, SLOT(onNmsThresholdSliderValueChanged(int)));

    nmsSliderLayout->addWidget(nmsMinLabel);
    nmsSliderLayout->addWidget(nmsThresholdSlider);
    nmsSliderLayout->addWidget(nmsMaxLabel);
    sliderLayout->addLayout(nmsSliderLayout);
    sliderLayout->addWidget(nmsValueLabel);

    QPushButton *startButton = new QPushButton("Start", this);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startClicked);
    sliderLayout->addWidget(startButton);

    QPushButton *stopButton = new QPushButton("Stop", this);
    connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopClicked);
    sliderLayout->addWidget(stopButton);

    topLayout->addLayout(sliderLayout);
    topLayout->addWidget(videoWidget);

    mainLayout->addLayout(topLayout);
    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {
    delete videoWidget;
}

void MainWindow::showFrame(const QImage &frame) {
    videoWidget->showFrame(frame);
}

void MainWindow::onConfThresholdSliderValueChanged(int value) {
    float confThreshold = static_cast<float>(value) / 100;
    emit confThresholdChanged(confThreshold);
}

void MainWindow::onNmsThresholdSliderValueChanged(int value) {
    float nmsThreshold = static_cast<float>(value) / 100;
    emit nmsThresholdChanged(nmsThreshold);
}

