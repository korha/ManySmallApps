#ifndef BEEPTEST_H
#define BEEPTEST_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStyle>
#include <QtCore/QThread>
#include <QtCore/qt_windows.h>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class Worker : public QObject        //***default constructor
{
    Q_OBJECT
public:
    inline void fSingle(const int iFreq, const int iDur)
    {
        iFrequency = iFreq;
        iDuration = iDur;
    }
    inline void fMultiple(const QVector<int> &vectSrc)
    {
        vectSource = vectSrc;
        iFrequency = 0;
    }

private:
    int iFrequency,
    iDuration;
    QVector<int> vectSource;

private slots:
    void process();

signals:
    void finished();
};

class BeepTest : public QWidget
{
    Q_OBJECT
public:
    BeepTest();
    ~BeepTest();

private:
    enum
    {
        eMinFreq = 37,
        eMaxFreq = 32767,
        eMinDuration = 1,
        eMaxDuration = 20000
    };
    QSpinBox *sbFrequency,
    *sbDuration;
    QPushButton *pbBeep;
    QPlainTextEdit *teSource;
    Worker worker;
    QThread *thread;

private slots:
    void slotBeep();
    inline void slotFinish()
    {
        thread->quit();
        pbBeep->setEnabled(true);
    }
};

#endif // BEEPTEST_H
