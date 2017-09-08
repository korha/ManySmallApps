#include "stopwatch.h"

//-------------------------------------------------------------------------------------------------
StopWatch::StopWatch() : QWidget(),
    timer(new QTimer(this)),
    strFormat("m:ss.zzz"),
    iChop(1)
{
    timer->setInterval(eIntervalMs);
    int iNumDigits = 8;

    //args
    const QStringList slistArgs = qApp->arguments();
    if (slistArgs.size() > 1)
        switch (slistArgs.at(1).toInt())
        {
        case 1:
        {
            strFormat = "hh:mm:ss";
            iChop = 0;
            //iNumDigits = 8;
            timer->setInterval(eIntervalSec);
            break;
        }
        case 2:
        {
            strFormat = "hh:mm:ss.zzz";
            iChop = 2;
            iNumDigits = 10;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 3:
        {
            strFormat = "hh:mm:ss.zzz";
            //iChop = 1;
            iNumDigits = 11;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 4:
        {
            strFormat = "hh:mm:ss.zzz";
            iChop = 0;
            iNumDigits = 12;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 5:
        {
            strFormat = "h:mm:ss";
            iChop = 0;
            //iNumDigits = 8;
            timer->setInterval(eIntervalSec);
            break;
        }
        case 6:
        {
            strFormat = "h:mm:ss.zzz";
            iChop = 2;
            iNumDigits = 10;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 7:
        {
            strFormat = "h:mm:ss.zzz";
            //iChop = 1;
            iNumDigits = 11;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 8:
        {
            strFormat = "h:mm:ss.zzz";
            iChop = 0;
            iNumDigits = 12;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 9:
        {
            strFormat = "mm:ss";
            iChop = 0;
            iNumDigits = 5;
            timer->setInterval(eIntervalSec);
            break;
        }
        case 10:
        {
            strFormat = "mm:ss.zzz";
            iChop = 2;
            iNumDigits = 7;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 11:
        {
            strFormat = "mm:ss.zzz";
            //iChop = 1;
            //iNumDigits = 8;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 12:
        {
            strFormat = "mm:ss.zzz";
            iChop = 0;
            iNumDigits = 9;
            //timer->setInterval(eIntervalMs);
            break;
        }
        case 13:
        {
            strFormat = "m:ss";
            iChop = 0;
            iNumDigits = 5;
            timer->setInterval(eIntervalSec);
            break;
        }
        case 14:
        {
            //strFormat = "m:ss.zzz";
            iChop = 2;
            iNumDigits = 7;
            //timer->setInterval(eIntervalMs);
            break;
        }
            //case 15:
            //{
            //strFormat = "m:ss.zzz";
            //iChop = 1;
            //iNumDigits = 8;
            //timer->setInterval(eIntervalMs);
            //break;
            //}
        case 16:
        {
            //strFormat = "m:ss.zzz";
            iChop = 0;
            iNumDigits = 9;
            //timer->setInterval(eIntervalMs);
            break;
        }
        }

    QString strTime = QTime(0, 0).toString(strFormat);
    strTime.chop(iChop);

    lcdNumb = new QLCDNumber(iNumDigits, this);
    lcdNumb->setFrameStyle(QFrame::NoFrame);
    lcdNumb->setFixedSize(eWidthDigitSegm*iNumDigits, eWidthDigitSegm*2);
    lcdNumb->display(strTime);
    pbStartStop = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Start", this);
    pbStartStop->setCheckable(true);
    pbCountdown = new QPushButton(style()->standardIcon(QStyle::SP_ArrowDown), "Countdown", this);
    pbCountdown->setEnabled(false);
    teList = new QPlainTextEdit(this);
    teList->setWordWrapMode(QTextOption::NoWrap);
    QPushButton *pbClean = new QPushButton(style()->standardIcon(QStyle::SP_DialogResetButton), "Clean", this);
    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->addWidget(lcdNumb, 0, Qt::AlignHCenter);
    vblMain->addWidget(pbStartStop);
    vblMain->addWidget(pbCountdown);
    vblMain->addWidget(teList);
    vblMain->addWidget(pbClean);

    //connects
    connect(pbStartStop, SIGNAL(clicked()), this, SLOT(slotStartStop()));
    connect(pbCountdown, SIGNAL(clicked()), this, SLOT(slotCountdown()));
    connect(pbClean, SIGNAL(clicked()), teList, SLOT(clear()));
    connect(timer, SIGNAL(timeout()), this, SLOT(slotShowTime()));

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
StopWatch::~StopWatch()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void StopWatch::slotStartStop()
{
    if (timer->isActive())
    {
        QString strTime = QTime(0, 0).addMSecs(time.elapsed()).toString(strFormat);
        strTime.chop(iChop);
        lcdNumb->display(strTime);
        teList->appendPlainText(strTime + '\n');
        timer->stop();
        pbStartStop->setIcon(style()->standardIcon(QStyle::SP_CommandLink));
        pbStartStop->setText("Start");
        pbCountdown->setEnabled(false);
    }
    else
    {
        time.start();
        timer->start();
        pbStartStop->setIcon(style()->standardIcon(QStyle::SP_LineEditClearButton));
        pbStartStop->setText("Stop");
        pbCountdown->setEnabled(true);
    }
}

//-------------------------------------------------------------------------------------------------
void StopWatch::slotCountdown() const
{
    QString strTime = QTime(0, 0).addMSecs(time.elapsed()).toString(strFormat);
    strTime.chop(iChop);
    teList->appendPlainText(strTime);
}

//-------------------------------------------------------------------------------------------------
void StopWatch::slotShowTime() const
{
    QString strTime = QTime(0, 0).addMSecs(time.elapsed()).toString(strFormat);
    strTime.chop(iChop);
    lcdNumb->display(strTime);
}
