// CloseEventFilter.h
#include <QObject>
#include <QEvent>
#include "MainWindow.h"

class CloseEventFilter : public QObject {
    Q_OBJECT

public:
    CloseEventFilter(QObject *parent = nullptr);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;
};
