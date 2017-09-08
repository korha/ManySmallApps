#include "macvendorlookup.h"

//-------------------------------------------------------------------------------------------------
MacVendorLookup::MacVendorLookup() : QWidget()
{
    char cError = '\0';
    const QString strAppPath = qApp->applicationFilePath();
    QFile file(strAppPath + ".db");
    int iTemp = file.size();
    if (iTemp > 500*1024 && iTemp < 15*1024*1024 && file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QByteArray baDb = file.readAll();
        if (!file.error())
        {
            file.close();
            if (!baDb.contains('>'))
            {
                iTemp = baDb.indexOf("\n\n00-");
                if (iTemp > 70)
                {
                    baDb.remove(0, iTemp+2);
                    if (baDb.right(2) == "\n\n")
                        baDb.chop(2);
                    baDb.replace("\n\n", ">");

                    QList<QByteArray> listDb = baDb.split('\n');
                    for (int i = listDb.size()-1; i >= 0; --i)
                        if (listDb.at(i).contains("(base 16)"))
                            listDb.removeAt(i);
                    baDb = listDb.join('\n');
                    baDb.replace('\t', "");
                    baDb.replace("   (hex)", "\n");

                    bool bOk;
                    const QStringList slistDb = QString(baDb).split('>');
                    for (int i = slistDb.size()-1; i >= 0; --i)
                    {
                        const QString &strRef = slistDb.at(i);
                        if (strRef.size() < 15 || strRef.at(2) != '-' || strRef.at(5) != '-' || strRef.at(8) != '\n')        //[16 (min) = "08-00-13\nEXXON\n"]
                        {
                            cError = '5';
                            break;
                        }
                        const uint iMac = (strRef.left(2) + strRef.mid(3, 2) + strRef.mid(6, 2)).toInt(&bOk, 16);
                        Q_ASSERT(iMac <= 0xFFFFFF);
                        if (!bOk)
                        {
                            cError = '6';
                            break;
                        }
                        map.insert(iMac, strRef);
                    }

                    if (!cError)
                    {
                        if (map.value(0x0001C8).contains("THOMAS"))        //check
                            map.insert(0x080030,        //multiply
                                       "08-00-30\n"
                                       "NETWORK RESEARCH CORPORATION\n"
                                       "2380 N. ROSE AVENUE\n"
                                       "OXNARD CA 93010\n"
                                       "US\n"
                                       "\n"
                                       "08-00-30\n"
                                       "ROYAL MELBOURNE INST OF TECH\n"
                                       "GPO BOX 2476V\n"
                                       "MELBOURNE VIC 3001\n"
                                       "AU\n"
                                       "\n"
                                       "08-00-30\n"
                                       "CERN\n"
                                       "CH-1211 GENEVE 23\n"
                                       "    SUISSE/SWITZ\n"
                                       "CH");
                        else
                            cError = '7';
                    }
                }
                else
                    cError = '4';
            }
            else
                cError = '3';
        }
        else
            cError = '2';
    }
    else
        cError = '1';

    leRawMac = new QLineEdit(this);
    leRawMac->setMaxLength(17);        //[17 = "12:45:78:0A:CD:F0"]
    leRawMac->setFixedWidth(leRawMac->fontMetrics().width("AAAAAAAAAAAAAAAAA") + 10);
    QPushButton *pbPaste = new QPushButton(QIcon(":/img/paste.png"), 0, this);
    QHBoxLayout *hblTop = new QHBoxLayout;
    hblTop->addWidget(leRawMac);
    hblTop->addWidget(pbPaste);
    hblTop->addStretch();

    lblNotes = new QLabel("?", this);
    lblNotes->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);

    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->addLayout(hblTop);
    vblMain->addWidget(lblNotes);
    vblMain->addStretch();

    file.setFileName(strAppPath + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
        file.close();
    }

    if (cError)
    {
        leRawMac->setEnabled(false);
        pbPaste->setEnabled(false);
        QMessageBox::warning(0, 0, QString("Error #") + cError);
    }
    else
    {
        //connects
        connect(leRawMac, SIGNAL(textEdited(QString)), this, SLOT(slotEdited(QString)));
        connect(pbPaste, SIGNAL(clicked()), this, SLOT(slotPaste()));

        //from clipboard
        QString strClpb = qApp->clipboard()->text();
        iTemp = strClpb.length();
        if (iTemp >= 6 && iTemp <= 17)
        {
            slotEdited(strClpb);
            Q_ASSERT(!lblNotes->text().isEmpty());
            if (lblNotes->text().at(0) != '?')
                leRawMac->setText(strClpb);
            leRawMac->selectAll();
        }
    }
}

//-------------------------------------------------------------------------------------------------
MacVendorLookup::~MacVendorLookup()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void MacVendorLookup::slotEdited(QString strMac)
{
    bool bOk = false;
    const uint iVal = strMac.remove(':').remove('-').length() >= 6 ? strMac.leftRef(6).toUInt(&bOk, 16) : 0;
    lblNotes->setText(bOk ? map.value(iVal, "?") : "?");
}

//-------------------------------------------------------------------------------------------------
void MacVendorLookup::slotPaste()
{
    const QString strClpb = qApp->clipboard()->text();
    leRawMac->setText(strClpb);
    slotEdited(strClpb);
}
