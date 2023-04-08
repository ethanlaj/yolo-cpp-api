#include "TimerHandler.h"

TimerHandler::TimerHandler()
{
    timer = new QTimer(this);
}

TimerHandler::~TimerHandler()
{
    delete timer;
}

void TimerHandler::startTimer(int interval)
{
    timer->start(interval);
}

void TimerHandler::stopTimer()
{
    timer->stop();
}
