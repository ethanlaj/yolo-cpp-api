#ifndef TIMERHANDLER_H
#define TIMERHANDLER_H

#include <QTimer>
#include <QObject>

class TimerHandler : public QObject
{
    Q_OBJECT
public:
    QTimer *timer;

    TimerHandler();
    ~TimerHandler();

public slots:
    void startTimer(int interval);
    void stopTimer();
};

#endif // TIMERHANDLER_H
