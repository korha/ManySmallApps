#include "basenumberconverter.h"

//-------------------------------------------------------------------------------------------------
BaseNumberConverter::BaseNumberConverter() : QTabWidget(),
    bLetterUp(true)
{
    this->findChild<QWidget*>("qt_tabwidget_stackedwidget", Qt::FindDirectChildrenOnly)->setAutoFillBackground(true);

    //
    QLabel *lblInput = new QLabel("Input:", this);
    leInput = new QLineEdit(this);

    QLabel *lblFrom = new QLabel(">>", this);

    rbFrom2 = new QRadioButton("2", this);
    rbFrom8 = new QRadioButton("8", this);
    rbFrom10 = new QRadioButton("10", this);
    rbFrom10->setChecked(true);
    QRadioButton *rbFrom16 = new QRadioButton("16", this);
    QHBoxLayout *hblFrom = new QHBoxLayout;
    hblFrom->addStretch();
    hblFrom->addWidget(rbFrom2);
    hblFrom->addWidget(rbFrom8);
    hblFrom->addWidget(rbFrom10);
    hblFrom->addWidget(rbFrom16);
    hblFrom->addStretch();

    QLabel *lblTo2 = new QLabel("2 <<", this);
    leTo2 = new QLineEdit("?", this);
    QLabel *lblTo8 = new QLabel("8 <<", this);
    leTo8 = new QLineEdit("?", this);
    QLabel *lblTo10 = new QLabel("10 <<", this);
    leTo10 = new QLineEdit("?", this);
    QLabel *lblTo16 = new QLabel("16 <<", this);
    leTo16 = new QLineEdit("?", this);

    QGridLayout *glInput = new QGridLayout;
    glInput->addWidget(lblInput, 0, 0, Qt::AlignRight);
    glInput->addWidget(leInput, 0, 1);
    glInput->addWidget(lblFrom, 1, 0, Qt::AlignRight);
    glInput->addLayout(hblFrom, 1, 1);
    glInput->addWidget(lblTo2, 2, 0, Qt::AlignRight);
    glInput->addWidget(leTo2, 2, 1);
    glInput->addWidget(lblTo8, 3, 0, Qt::AlignRight);
    glInput->addWidget(leTo8, 3, 1);
    glInput->addWidget(lblTo10, 4, 0, Qt::AlignRight);
    glInput->addWidget(leTo10, 4, 1);
    glInput->addWidget(lblTo16, 5, 0, Qt::AlignRight);
    glInput->addWidget(leTo16, 5, 1);

    QWidget *wgtInput = new QWidget(this);
    QVBoxLayout *vblInput = new QVBoxLayout(wgtInput);
    vblInput->addLayout(glInput);
    vblInput->addStretch();

    //
    QLabel *lblIn = new QLabel("From:", this);
    sbIn = new QSpinBox(this);
    sbIn->setRange(2, 36);
    sbIn->setValue(10);
    QPushButton *pbConversion = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Conversion", this);
    QLabel *lblOut = new QLabel("To:", this);
    sbOut = new QSpinBox(this);
    sbOut->setRange(2, 36);
    sbOut->setValue(16);

    teIn = new QPlainTextEdit(this);
    teIn->setWordWrapMode(QTextOption::NoWrap);
    teOut = new QPlainTextEdit(this);
    teOut->setWordWrapMode(QTextOption::NoWrap);
    QHBoxLayout *hblText = new QHBoxLayout;
    hblText->addWidget(teIn);
    hblText->addWidget(teOut);

    QHBoxLayout *hblConversion = new QHBoxLayout;
    hblConversion->addStretch();
    hblConversion->addWidget(lblIn);
    hblConversion->addWidget(sbIn);
    hblConversion->addWidget(pbConversion);
    hblConversion->addWidget(lblOut);
    hblConversion->addWidget(sbOut);
    hblConversion->addStretch();

    QWidget *wgtConversion = new QWidget(this);
    QVBoxLayout *vblConversion = new QVBoxLayout(wgtConversion);
    vblConversion->addLayout(hblConversion);
    vblConversion->addLayout(hblText);

    this->addTab(wgtInput, "Input");
    this->addTab(wgtConversion, "List");
    this->setMinimumSize(this->minimumSizeHint());
    leInput->setFocus();

    //connects
    connect(leInput, SIGNAL(textEdited(QString)), this, SLOT(slotInputEdited(QString)));
    connect(rbFrom2, SIGNAL(toggled(bool)), this, SLOT(slotChangeBase(bool)));
    connect(rbFrom8, SIGNAL(toggled(bool)), this, SLOT(slotChangeBase(bool)));
    connect(rbFrom10, SIGNAL(toggled(bool)), this, SLOT(slotChangeBase(bool)));
    connect(rbFrom16, SIGNAL(toggled(bool)), this, SLOT(slotChangeBase(bool)));
    connect(pbConversion, SIGNAL(clicked()), this, SLOT(slotConversion()));

    //arguments
    const QStringList slistArgs = qApp->arguments();
    for (int i = 1; i < slistArgs.size(); ++i)
    {
        const QString &strArg = slistArgs.at(i);
        if (strArg == "/case:low")
            bLetterUp = false;
        else if (strArg == "/tab:list")
            this->setCurrentIndex(1);
        else if (strArg.startsWith("/from:"))
        {
            const int iFrom = strArg.midRef(6).toInt();
            if (iFrom >= 2 && iFrom <= 36)
                sbIn->setValue(iFrom);
        }
        else if (strArg.startsWith("/to:"))
        {
            const int iTo = strArg.midRef(4).toInt();
            if (iTo >= 2 && iTo <= 36)
                sbOut->setValue(iTo);
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
BaseNumberConverter::~BaseNumberConverter()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void BaseNumberConverter::slotChangeBase(bool bToggle)
{
    if (bToggle)
        slotInputEdited(leInput->text());
}

//-------------------------------------------------------------------------------------------------
void BaseNumberConverter::slotInputEdited(const QString &strInput)
{
    bool bOk;
    const qulonglong iNumber = strInput.toULongLong(&bOk, rbFrom2->isChecked() ? 2 : (rbFrom8->isChecked() ? 8 : (rbFrom10->isChecked() ? 10 : 16)));
    if (bOk)
    {
        leTo2->setText(QString::number(iNumber, 2));
        leTo8->setText(QString::number(iNumber, 8));
        leTo10->setText(QString::number(iNumber, 10));
        leTo16->setText(bLetterUp ? QString::number(iNumber, 16).toUpper() : QString::number(iNumber, 16));
    }
    else
    {
        leTo2->setText("?");
        leTo8->setText("?");
        leTo10->setText("?");
        leTo16->setText("?");
    }
}

//-------------------------------------------------------------------------------------------------
void BaseNumberConverter::slotConversion()
{
    const int iIn = sbIn->value(),
            iOut = sbOut->value();
    const QStringList slistIn = teIn->toPlainText().split('\n');
    bool bOk;
    qulonglong iNumber;
    QString strOut;
    for (int i = 0; i < slistIn.size(); ++i)
    {
        iNumber = slistIn.at(i).toULongLong(&bOk, iIn);
        if (bOk)
            strOut += bLetterUp ? QString::number(iNumber, iOut).toUpper() : QString::number(iNumber, iOut);
        strOut += '\n';
    }
    strOut.chop(1);
    teOut->setPlainText(strOut);
}
