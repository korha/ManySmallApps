#include "beeptest.h"

//-------------------------------------------------------------------------------------------------
void Worker::process()
{
    Q_ASSERT(vectSource.size()%2 == 0);
    if (iFrequency)
        ::Beep(iFrequency, iDuration);
    else
        for (int i = 0; i < vectSource.size(); i+=2)
            ::Beep(vectSource.at(i), vectSource.at(i+1));
    finished();
}

//-------------------------------------------------------------------------------------------------
BeepTest::BeepTest() : QWidget(),
    thread(new QThread(this))
{
    QLabel *lblFrequency = new QLabel("Frequency (hz):", this);
    sbFrequency = new QSpinBox(this);
    sbFrequency->setRange(eMinFreq, eMaxFreq);
    sbFrequency->setValue(1000);
    QLabel *lblDuration = new QLabel("Duration (ms):", this);
    sbDuration = new QSpinBox(this);
    sbDuration->setRange(eMinDuration, eMaxDuration);
    sbDuration->setValue(1000);

    QGridLayout *glBlock = new QGridLayout;
    glBlock->addWidget(lblFrequency, 0, 0);
    glBlock->addWidget(sbFrequency, 0, 1);
    glBlock->addWidget(lblDuration, 1, 0);
    glBlock->addWidget(sbDuration, 1, 1);

    QHBoxLayout *hblBlock = new QHBoxLayout;
    hblBlock->addLayout(glBlock);
    hblBlock->addStretch();

    pbBeep = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Beep", this);

    teSource = new QPlainTextEdit(this);

    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->addLayout(hblBlock);
    vblMain->addWidget(pbBeep);
    vblMain->addWidget(teSource);

    worker.moveToThread(thread);

    //connects
    connect(pbBeep, SIGNAL(clicked()), this, SLOT(slotBeep()));
    connect(thread, SIGNAL(started()), &worker, SLOT(process()));
    connect(&worker, SIGNAL(finished()), this, SLOT(slotFinish()));

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
BeepTest::~BeepTest()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
    thread->quit();
    thread->wait();
}

//-------------------------------------------------------------------------------------------------
void BeepTest::slotBeep()
{
    const QString strSource = teSource->toPlainText();
    if (strSource.isEmpty())
    {
        pbBeep->setEnabled(false);
        worker.fSingle(sbFrequency->value(), sbDuration->value());
        thread->start();
    }
    else
    {
        const QStringList slistSource = strSource.split('\n', QString::SkipEmptyParts);
        QVector<int> vectSource(slistSource.size()*2);
        bool bError = false;
        for (int i = 0; i < slistSource.size(); ++i)
        {
            int iTemp = slistSource.at(i).indexOf(',');
            if (iTemp > 1)
            {
                const int iFreq = slistSource.at(i).leftRef(iTemp).toInt();
                iTemp = slistSource.at(i).midRef(iTemp+1).toInt();
                if (iFreq >= eMinFreq && iFreq <= eMaxFreq && iTemp >= eMinDuration && iTemp <= eMaxDuration)
                {
                    vectSource[i*2] = iFreq;
                    vectSource[i*2+1] = iTemp;
                }
                else
                {
                    bError = true;
                    break;
                }
            }
            else
            {
                bError = true;
                break;
            }
        }

        if (bError)
            QMessageBox::warning(this, 0, "Incorrect");
        else
        {
            pbBeep->setEnabled(false);
            worker.fMultiple(vectSource);
            thread->start();
        }
    }
}
