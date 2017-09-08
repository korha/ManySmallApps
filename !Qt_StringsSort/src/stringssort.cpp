#include "stringssort.h"

//-------------------------------------------------------------------------------------------------
StringsSort::StringsSort() : QWidget()
{
    teText = new QPlainTextEdit(this);
    teText->setWordWrapMode(QTextOption::NoWrap);

    QPushButton *pbPaste = new QPushButton(QIcon(":/img/paste.png"), 0, this);
    QPushButton *pbMake = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Sort", this);
    QPushButton *pbCopy = new QPushButton(QIcon(":/img/copy.png"), 0, this);
    QHBoxLayout *hblAct = new QHBoxLayout;
    hblAct->addWidget(pbPaste);
    hblAct->addWidget(pbMake, 1);
    hblAct->addWidget(pbCopy);

    rbAZ = new QRadioButton("A->Z", this);
    rbAZ->setChecked(true);
    rbAZ->setToolTip("Ascending");
    QRadioButton *rbZA = new QRadioButton("Z->A", this);
    rbZA->setToolTip("Descending");
    chbDuplicates = new QCheckBox("Remove duplicates", this);
    lblCount = new QLabel(this);
    QHBoxLayout *hblOpt = new QHBoxLayout;
    hblOpt->addWidget(rbAZ);
    hblOpt->addWidget(rbZA);
    hblOpt->addWidget(chbDuplicates, 1);
    hblOpt->addWidget(lblCount);

    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->setContentsMargins(5, 5, 5, 5);
    vblMain->addWidget(teText);
    vblMain->addLayout(hblAct);
    vblMain->addLayout(hblOpt);

    //connects
    connect(pbPaste, SIGNAL(clicked()), this, SLOT(slotPaste()));
    connect(pbMake, SIGNAL(clicked()), this, SLOT(slotMake()));
    connect(pbCopy, SIGNAL(clicked()), this, SLOT(slotCopy()));

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
StringsSort::~StringsSort()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void StringsSort::slotPaste() const
{
    const QString strPaste = qApp->clipboard()->text();
    if (!strPaste.isEmpty())
        teText->setPlainText(strPaste);
}

//-------------------------------------------------------------------------------------------------
void StringsSort::slotMake() const
{
    QStringList slistLines = teText->toPlainText().split('\n', QString::SkipEmptyParts);
    if (chbDuplicates->isChecked())
        lblCount->setText(QString::number(slistLines.removeDuplicates()));
    else
        lblCount->clear();
    qSort(slistLines.begin(), slistLines.end(), rbAZ->isChecked() ? fCompareLess : fCompareGreater);
    teText->setPlainText(slistLines.join('\n'));
}

//-------------------------------------------------------------------------------------------------
void StringsSort::slotCopy() const
{
    const QString strCopy = teText->toPlainText();
    if (!strCopy.isEmpty())
        qApp->clipboard()->setText(strCopy);
}
