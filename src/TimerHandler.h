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
