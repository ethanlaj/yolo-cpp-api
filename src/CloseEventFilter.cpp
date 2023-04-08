// CloseEventFilter.cpp
#include "CloseEventFilter.h"

CloseEventFilter::CloseEventFilter(QObject *parent) : QObject(parent) {}

bool CloseEventFilter::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::Close) {
        MainWindow *mainWindow = qobject_cast<MainWindow *>(obj);
        if (mainWindow) {
            mainWindow->stopVideoProcessing();
        }
    }
    return QObject::eventFilter(obj, event);
}
