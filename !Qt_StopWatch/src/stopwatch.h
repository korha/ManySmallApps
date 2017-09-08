#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QLCDNumber>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QVBoxLayout>
#include <QtCore/QTime>
#include <QtCore/QTimer>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class StopWatch : public QWidget
{
    Q_OBJECT
public:
    StopWatch();
    ~StopWatch();

private:
    enum
    {
        eIntervalMs = 100,
        eIntervalSec = 400,
        eWidthDigitSegm = 40
    };
    QLCDNumber *lcdNumb;
    QPushButton *pbStartStop,
    *pbCountdown;
    QPlainTextEdit *teList;
    QTimer *timer;
    QTime time;
    QString strFormat;
    int iChop;

private slots:
    void slotStartStop();
    void slotCountdown() const;
    void slotShowTime() const;
};

#endif // STOPWATCH_H
