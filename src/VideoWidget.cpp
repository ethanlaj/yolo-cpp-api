#include "VideoWidget.h"

VideoWidget::VideoWidget(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);
    imageLabel = new QLabel(this);
    layout->addWidget(imageLabel);
}

void VideoWidget::showFrame(const QImage &frame) {
    imageLabel->setPixmap(QPixmap::fromImage(frame));
}
