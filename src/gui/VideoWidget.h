// VideoWidget.h

#ifndef VIDEOWIDGET_H
#define VIDEOWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QImage>

class VideoWidget : public QWidget {
    Q_OBJECT

public:
    explicit VideoWidget(QWidget *parent = nullptr);
    
    virtual ~VideoWidget() {}

    void showFrame(const QImage &frame);

private:
    QLabel *imageLabel;
};

#endif // VIDEOWIDGET_H
