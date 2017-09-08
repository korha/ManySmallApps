#include "sattoplaylist.h"

//-------------------------------------------------------------------------------------------------
SatToPlaylist::SatToPlaylist() : QWidget()
{
    QLabel *lblAddress = new QLabel("IP:Port", this);
    leAddress = new QLineEdit(this);
    leAddress->setPlaceholderText("192.168.100.10:8080");
    QHBoxLayout *hblIpPort = new QHBoxLayout;
    hblIpPort->addWidget(lblAddress);
    hblIpPort->addWidget(leAddress);

    QPushButton *pbOpen = new QPushButton(style()->standardIcon(QStyle::SP_DirOpenIcon), "Open sources", this);

    pbExportM3u = new QPushButton(QIcon(":/img/vlc.png"), "Export .m3u", this);
    pbExportM3u->setEnabled(false);
    pbExportMpcpl = new QPushButton(QIcon(":/img/mpc.png"), "Export .mpcpl", this);
    pbExportMpcpl->setEnabled(false);
    QHBoxLayout *hblExport = new QHBoxLayout;
    hblExport->addWidget(pbExportM3u);
    hblExport->addWidget(pbExportMpcpl);

    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->addLayout(hblIpPort);
    vblMain->addWidget(pbOpen, 0, Qt::AlignHCenter);
    vblMain->addLayout(hblExport);

    this->setFixedSize(this->minimumSizeHint());

    //conects
    connect(pbOpen, SIGNAL(clicked()), this, SLOT(slotOpen()));
    connect(pbExportM3u, SIGNAL(clicked()), this, SLOT(slotExportM3u()));
    connect(pbExportMpcpl, SIGNAL(clicked()), this, SLOT(slotExportMpcpl()));

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
SatToPlaylist::~SatToPlaylist()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void SatToPlaylist::slotOpen()
{
    Q_ASSERT(!leAddress->placeholderText().isEmpty());
    const QStringList slistFiles = QFileDialog::getOpenFileNames(this);
    if (!slistFiles.empty())
    {
        pbExportM3u->setEnabled(false);
        pbExportMpcpl->setEnabled(false);
        QFile file;
        QStringList slistLines;
        for (int i = 0; i < slistFiles.size(); ++i)
        {
            file.setFileName(slistFiles.at(i));
            if (file.open(QIODevice::ReadOnly | QIODevice::Text))
            {
                slistLines.append(QString(file.readAll()).split('\n'));
                if (file.error())
                {
                    QMessageBox::warning(this, 0, "Error read file:\n" + file.errorString());
                    return;
                }
                file.close();
            }
            else
            {
                QMessageBox::warning(this, 0, "Error open file:\n" + slistFiles.at(i));
                return;
            }
        }
        QString strAddress = leAddress->text();
        if (strAddress.isEmpty())
            strAddress = leAddress->placeholderText();
        strAddress.prepend("http://");
        strAddress += '/';

        QString strM3u = "#EXTM3U",
                strMpcpl = "MPCPLAYLIST";
        int iCount = 0;
        for (int i = 0; i < slistLines.size(); ++i)
        {
            const QString &strLine = slistLines.at(i);
            if (strLine.startsWith("#SERVICE "))
            {
                const int iDelim = strLine.lastIndexOf("::");
                if (iDelim > 21)        //[21 = "#SERVICE 0:0:0:0:0:0:"]
                {
                    const QString strUrl = strAddress + strLine.mid(9, iDelim-8),        //[9 = "#SERVICE "]
                            strLabel = strLine.mid(iDelim+2);
                    if (strLabel.isEmpty())
                    {
                        QMessageBox::warning(this, 0, "Empty label");
                        return;
                    }

                    strM3u += "\n#EXTINF:-1," + strLabel + '\n' + strUrl;

                    const QString strNum = '\n' + QString::number(++iCount) + ',';
                    strMpcpl += strNum + "type,0" + strNum + "label," + strLabel + strNum + "filename," + strUrl;
                }
                else
                {
                    QMessageBox::warning(this, 0, "Error file structure");
                    return;
                }
            }
        }
        if (iCount)
        {
            strM3u += '\n';
            strMpcpl += '\n';
            baM3u = strM3u.toUtf8();
            baMpcpl = strMpcpl.toUtf8();
            pbExportM3u->setEnabled(true);
            pbExportMpcpl->setEnabled(true);
        }
        else
            QMessageBox::warning(this, 0, "Error file structure");
    }
}

//-------------------------------------------------------------------------------------------------
void SatToPlaylist::slotExportM3u()
{
    const QString strPath = QFileDialog::getSaveFileName(this, 0, 0, "Playlist (*.m3u);;All files (*.*)");
    if (!strPath.isEmpty())
    {
        QFile file(strPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(baM3u);
            if (file.error())
                QMessageBox::warning(this, 0, "Error save file:\n" + file.error());
        }
        else
            QMessageBox::warning(this, 0, "Error open file");
    }
}

//-------------------------------------------------------------------------------------------------
void SatToPlaylist::slotExportMpcpl()
{
    const QString strPath = QFileDialog::getSaveFileName(this, 0, 0, "Playlist (*.mpcpl);;All files (*.*)");
    if (!strPath.isEmpty())
    {
        QFile file(strPath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            file.write(baMpcpl);
            if (file.error())
                QMessageBox::warning(this, 0, "Error save file:\n" + file.error());
        }
        else
            QMessageBox::warning(this, 0, "Error open file");
    }
}
