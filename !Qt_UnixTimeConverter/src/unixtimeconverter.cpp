#include "unixtimeconverter.h"

#define MIN_TIMESTAMP -2208816000000LL
#define MAX_TIMESTAMP 253402041600000LL
const char *const pDefaultFormat = "yyyy.MM.dd hh:mm:ss";

//-------------------------------------------------------------------------------------------------
UnixTimeConverter::UnixTimeConverter() : QWidget()
{
    Q_ASSERT(sizeof(qlonglong) == sizeof(qint64));
    Q_ASSERT(QDateTime(QDate(1900,  1,  3), QTime(), Qt::UTC).toMSecsSinceEpoch() == MIN_TIMESTAMP);
    Q_ASSERT(QDateTime(QDate(9999, 12, 29), QTime(), Qt::UTC).toMSecsSinceEpoch() == MAX_TIMESTAMP);

    QLabel *lblUnixUH = new QLabel("Unix Time:", this);
    leUnixUH = new QLineEdit(this);
    chbMSecUH = new QCheckBox("ms", this);
    QHBoxLayout *hblUnixMSecUH = new QHBoxLayout;
    hblUnixMSecUH->addWidget(leUnixUH);
    hblUnixMSecUH->addWidget(chbMSecUH);
    QPushButton *pbUnixPasteUH = new QPushButton(QIcon(":/img/paste.png"), 0, this);
    QLabel *lblFormatUH = new QLabel("Format:", this);
    leFormatUH = new QLineEdit(this);
    leFormatUH->setPlaceholderText(pDefaultFormat);

    QPushButton *pbDecodeUH = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Decode", this);

    QLabel *lblLocalUH = new QLabel("Local:", this);
    leLocalUH = new QLineEdit(this);
    QPushButton *pbLocalCopyUH = new QPushButton(QIcon(":/img/copy.png"), 0, this);

    QLabel *lblUTCUH = new QLabel("UTC:", this);
    leUTCUH = new QLineEdit(this);
    QPushButton *pbUTCCopyUH = new QPushButton(QIcon(":/img/copy.png"), 0, this);

    QWidget *wgtUH = new QWidget(this);
    QGridLayout *glUH = new QGridLayout(wgtUH);
    glUH->addWidget(lblUnixUH, 0, 0);
    glUH->addLayout(hblUnixMSecUH, 0, 1);
    glUH->addWidget(pbUnixPasteUH, 0, 2);
    glUH->addWidget(lblFormatUH, 1, 0);
    glUH->addWidget(leFormatUH, 1, 1);
    glUH->addWidget(pbDecodeUH, 2, 1);
    glUH->addWidget(lblLocalUH, 3, 0);
    glUH->addWidget(leLocalUH, 3, 1);
    glUH->addWidget(pbLocalCopyUH, 3, 2);
    glUH->addWidget(lblUTCUH, 4, 0);
    glUH->addWidget(leUTCUH, 4, 1);
    glUH->addWidget(pbUTCCopyUH, 4, 2);

    //
    QLabel *lblDateHU = new QLabel("Date:", this);
    leDateHU = new QLineEdit(this);
    QPushButton *pbDatePasteHU = new QPushButton(QIcon(":/img/paste.png"), 0, this);

    QLabel *lblFormatHU = new QLabel("Format:", this);
    leFormatHU = new QLineEdit(this);
    leFormatHU->setPlaceholderText(pDefaultFormat);

    QPushButton *pbEncodeHU = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Encode", this);

    QLabel *lblUnixHU = new QLabel("Unix Time:", this);
    leUnixHU = new QLineEdit(this);
    chbMSecHU = new QCheckBox("ms", this);
    QHBoxLayout *hblUnixMSecHU = new QHBoxLayout;
    hblUnixMSecHU->addWidget(leUnixHU);
    hblUnixMSecHU->addWidget(chbMSecHU);
    QPushButton *pbUnixCopyHU = new QPushButton(QIcon(":/img/copy.png"), 0, this);

    QVBoxLayout *vblSpacing = new QVBoxLayout;
    vblSpacing->addStretch();

    QWidget *wgtHU = new QWidget(this);
    QGridLayout *glHU = new QGridLayout(wgtHU);
    glHU->addWidget(lblDateHU, 0, 0);
    glHU->addWidget(leDateHU, 0, 1);
    glHU->addWidget(pbDatePasteHU, 0, 2);
    glHU->addWidget(lblFormatHU, 1, 0);
    glHU->addWidget(leFormatHU, 1, 1);
    glHU->addWidget(pbEncodeHU, 2, 1);
    glHU->addWidget(lblUnixHU, 3, 0);
    glHU->addLayout(hblUnixMSecHU, 3, 1);
    glHU->addWidget(pbUnixCopyHU, 3, 2);
    glHU->addLayout(vblSpacing, 4, 0);

    //
    twMain = new QTabWidget(this);
    twMain->findChild<QWidget*>("qt_tabwidget_stackedwidget", Qt::FindDirectChildrenOnly)->setAutoFillBackground(true);
    twMain->addTab(wgtUH, "Unix Time -> Human");
    twMain->addTab(wgtHU, "Human -> Unix Time");

    //
    QPushButton *pbNow = new QPushButton(style()->standardIcon(QStyle::SP_BrowserReload), "Now", this);
    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->setContentsMargins(0, 0, 0, 0);
    vblMain->setSpacing(0);
    vblMain->addWidget(twMain);
    vblMain->addWidget(pbNow, 0, Qt::AlignRight);

    this->setMaximumHeight(this->minimumSizeHint().height());

    strLocaleInTime = "Incorrect Unix Time";
    strLocaleInDate = "Incorrect date";

    //connects
    connect(pbUnixPasteUH, SIGNAL(clicked()), this, SLOT(slotUnixPaste()));
    connect(pbDecodeUH, SIGNAL(clicked()), this, SLOT(slotDecode()));
    connect(pbLocalCopyUH, SIGNAL(clicked()), this, SLOT(slotLocalCopy()));
    connect(pbUTCCopyUH, SIGNAL(clicked()), this, SLOT(slotUTCCopy()));
    connect(pbDatePasteHU, SIGNAL(clicked()), this, SLOT(slotDatePaste()));
    connect(pbEncodeHU, SIGNAL(clicked()), this, SLOT(slotEncode()));
    connect(pbUnixCopyHU, SIGNAL(clicked()), this, SLOT(slotUnixCopy()));
    connect(pbNow, SIGNAL(clicked()), this, SLOT(slotNow()));

    //arguments
    const QStringList slistArgs = qApp->arguments();
    for (int i = 1; i < slistArgs.size(); ++i)
    {
        const QString &strArg = slistArgs.at(i);
        if (strArg == "/tab:date")
            twMain->setCurrentIndex(1);
        else if (strArg == "/msec:1")
        {
            chbMSecUH->setChecked(true);
            chbMSecHU->setChecked(true);
        }
        else if (strArg.startsWith("/format:"))
        {
            const QString strFormat = strArg.mid(8);
            leFormatUH->setText(strFormat);
            leFormatHU->setText(strFormat);
        }
    }

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
UnixTimeConverter::~UnixTimeConverter()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotUnixPaste()
{
    const QString strUnixUH = qApp->clipboard()->text();
    if (!strUnixUH.isEmpty())
    {
        leUnixUH->setText(strUnixUH);
        slotDecode();
    }
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotDecode()
{
    const bool bMSec = chbMSecUH->isChecked();
    bool bOk;
    qint64 iUnix = leUnixUH->text().toLongLong(&bOk);
    if (bOk && (bMSec ?
                (iUnix >= MIN_TIMESTAMP && iUnix <= MAX_TIMESTAMP) :
                (iUnix >= MIN_TIMESTAMP/1000 && iUnix <= MAX_TIMESTAMP/1000)))
    {
        if (!bMSec)
            iUnix *= 1000;
        QString strFormat = leFormatUH->text();
        if (strFormat.isEmpty())
            strFormat = pDefaultFormat;;
        leLocalUH->setText(QDateTime::fromMSecsSinceEpoch(iUnix).toString(strFormat));
        leUTCUH->setText(QDateTime::fromMSecsSinceEpoch(iUnix, Qt::UTC).toString(strFormat));
    }
    else
    {
        leLocalUH->clear();
        leUTCUH->clear();
        leUnixUH->setFocus();
        QMessageBox::warning(this, 0, strLocaleInTime);
    }
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotLocalCopy() const
{
    const QString strLocalUH = leLocalUH->text();
    if (!strLocalUH.isEmpty())
        qApp->clipboard()->setText(strLocalUH);
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotUTCCopy() const
{
    const QString strUTCUH = leUTCUH->text();
    if (!strUTCUH.isEmpty())
        qApp->clipboard()->setText(strUTCUH);
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotDatePaste()
{
    const QString strDateHU = qApp->clipboard()->text();
    if (!strDateHU.isEmpty())
    {
        leDateHU->setText(strDateHU);
        slotEncode();
    }
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotEncode()
{
    QString strFormat = leFormatHU->text();
    if (strFormat.isEmpty())
        strFormat = pDefaultFormat;;
    const QDateTime dt = QDateTime::fromString(leDateHU->text(), strFormat);
    if (dt.isValid())
    {
        const qint64 iUnix = dt.toMSecsSinceEpoch();
        leUnixHU->setText(QString::number(chbMSecHU->isChecked() ? iUnix : iUnix/1000));
    }
    else
    {
        leUnixHU->clear();
        leDateHU->setFocus();
        QMessageBox::warning(this, 0, strLocaleInDate);
    }
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotUnixCopy() const
{
    const QString strUnixHU = leUnixHU->text();
    if (!strUnixHU.isEmpty())
        qApp->clipboard()->setText(strUnixHU);
}

//-------------------------------------------------------------------------------------------------
void UnixTimeConverter::slotNow()
{
    if (twMain->currentIndex() == 0)
    {
        const qint64 iUnix = QDateTime::currentMSecsSinceEpoch();
        leUnixUH->setText(QString::number(chbMSecUH->isChecked() ? iUnix : iUnix/1000));
        slotDecode();
    }
    else
    {
        QString strFormat = leFormatHU->text();
        if (strFormat.isEmpty())
            strFormat = pDefaultFormat;;
        leDateHU->setText(QDateTime::currentDateTime().toString(strFormat));
        slotEncode();
    }
}
