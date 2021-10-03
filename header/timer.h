#ifndef TIMER_H
#define TIMER_H

#include <QLabel>
#include <QObject>

class Timer : public QLabel
{
    Q_OBJECT
public:
    Timer(QWidget *parent = nullptr);
};

#endif // TIMER_H
