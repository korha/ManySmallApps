#include "base64converter.h"

//-------------------------------------------------------------------------------------------------
TextEditEx::TextEditEx(QWidget *parent) : QPlainTextEdit(parent)
{
    this->setWordWrapMode(QTextOption::WrapAnywhere);
}

//-------------------------------------------------------------------------------------------------
void TextEditEx::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls() && QFileInfo(event->mimeData()->urls().first().toLocalFile()).isFile())
        event->acceptProposedAction();
}

//-------------------------------------------------------------------------------------------------
void TextEditEx::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();        //avoid hold cursor
}

//-------------------------------------------------------------------------------------------------
void TextEditEx::dropEvent(QDropEvent *event)
{
    signalToBase64(event->mimeData()->urls().first().toLocalFile());
}

//-------------------------------------------------------------------------------------------------
Base64Converter::Base64Converter() : QTabWidget()
{
    this->findChild<QWidget*>("qt_tabwidget_stackedwidget", Qt::FindDirectChildrenOnly)->setAutoFillBackground(true);

    //
    QLabel *lblText = new QLabel("Plain Text:", this);

    tePlain = new QPlainTextEdit(this);
    tePlain->setWordWrapMode(QTextOption::NoWrap);

    QPushButton *pbEncode = new QPushButton(style()->standardIcon(QStyle::SP_ArrowDown), "Encode", this);
    QLabel *lblBase = new QLabel("Base64:", this);
    QPushButton *pbDecode = new QPushButton(style()->standardIcon(QStyle::SP_ArrowUp), "Decode", this);
    QHBoxLayout *hblAction = new QHBoxLayout;
    hblAction->addWidget(pbEncode, 0, Qt::AlignLeft);
    hblAction->addWidget(lblBase, 0, Qt::AlignHCenter);
    hblAction->addWidget(pbDecode, 0, Qt::AlignRight);

    teBase = new QPlainTextEdit(this);
    teBase->setWordWrapMode(QTextOption::WrapAnywhere);

    QWidget *wgtText = new QWidget(this);
    QVBoxLayout *vblText = new QVBoxLayout(wgtText);
    vblText->setContentsMargins(5, 5, 5, 5);
    vblText->addWidget(lblText, 0, Qt::AlignHCenter);
    vblText->addWidget(tePlain);
    vblText->addLayout(hblAction);
    vblText->addWidget(teBase);

    //
    chbMimeType = new QCheckBox("MIME-Type", this);
    chbMimeType->setChecked(true);

    teEditEx = new TextEditEx(this);

    QPushButton *pbPaste = new QPushButton(QIcon(":/img/paste.png"), 0, this);
    QPushButton *pbOpen = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), "Open", this);
    QPushButton *pbSave = new QPushButton(style()->standardIcon(QStyle::SP_DialogSaveButton), "Save", this);
    QPushButton *pbCopy = new QPushButton(QIcon(":/img/copy.png"), 0, this);
    QHBoxLayout *hblDialog = new QHBoxLayout;
    hblDialog->addWidget(pbPaste);
    hblDialog->addWidget(pbOpen);
    hblDialog->addStretch();
    hblDialog->addWidget(pbSave);
    hblDialog->addWidget(pbCopy);

    QWidget *wgtFile = new QWidget(this);
    QVBoxLayout *vblFile = new QVBoxLayout(wgtFile);
    vblFile->setContentsMargins(5, 5, 5, 5);
    vblFile->addWidget(chbMimeType);
    vblFile->addWidget(teEditEx);
    vblFile->addLayout(hblDialog);

    //
    this->addTab(wgtText, "Plain Text / Base64");
    this->addTab(wgtFile, "File / Base64");
    this->setMinimumSize(this->minimumSizeHint());
    tePlain->setFocus();

    //connects
    connect(pbEncode, SIGNAL(clicked()), this, SLOT(slotEncode()));
    connect(pbDecode, SIGNAL(clicked()), this, SLOT(slotDecode()));
    connect(pbPaste, SIGNAL(clicked()), this, SLOT(slotPaste()));
    connect(pbOpen, SIGNAL(clicked()), this, SLOT(slotOpen()));
    connect(pbSave, SIGNAL(clicked()), this, SLOT(slotSave()));
    connect(pbCopy, SIGNAL(clicked()), this, SLOT(slotCopy()));
    connect(teEditEx, SIGNAL(signalToBase64(QString)), this, SLOT(slotToBase64(QString)));

    //arguments
    const QStringList slistArgs = qApp->arguments();
    for (int i = 1; i < slistArgs.size(); ++i)
    {
        const QString &strArg = slistArgs.at(i);
        if (strArg == "/tab:file")
            this->setCurrentIndex(1);
        else if (strArg == "/mimetype:0")
            chbMimeType->setChecked(false);
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
Base64Converter::~Base64Converter()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotEncode() const
{
    teBase->setPlainText(tePlain->toPlainText().toUtf8().toBase64());
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotDecode() const
{
    tePlain->setPlainText(QByteArray::fromBase64(teBase->toPlainText().toUtf8()));
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotPaste() const
{
    const QString strPaste = qApp->clipboard()->text();
    if (!strPaste.isEmpty())
        teEditEx->setPlainText(strPaste);
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotOpen()
{
    const QString strFilePath = QFileDialog::getOpenFileName(this);
    if (!strFilePath.isEmpty())
        slotToBase64(strFilePath);
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotSave()
{
    const QString strFilePath = QFileDialog::getSaveFileName(this);
    if (strFilePath.isEmpty())
        return;
    QFile file(strFilePath);
    if (!file.open(QIODevice::WriteOnly))
    {
        QMessageBox::warning(this, 0, "Can't create file");
        return;
    }
    const QByteArray baBase64 = teEditEx->toPlainText().toUtf8();
    if (baBase64.startsWith("data:"))        //Discard MIME-Type
    {
        const int iDelim = baBase64.indexOf(',', 5);        //[5 = data:] [, - delimeter between MIME-Type and Base64 code]
        if (iDelim < 0)
        {
            QMessageBox::warning(this, 0, "Incorrect Base64 code");
            return;
        }
        file.write(QByteArray::fromBase64(baBase64.mid(iDelim)));
    }
    else
        file.write(QByteArray::fromBase64(baBase64));
    if (file.error())
        QMessageBox::warning(this, 0, file.errorString());
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotCopy() const
{
    const QString strCopy = teEditEx->toPlainText();
    if (!strCopy.isEmpty())
        qApp->clipboard()->setText(strCopy);
}

//-------------------------------------------------------------------------------------------------
void Base64Converter::slotToBase64(const QString &strFilePath)
{
    QFile file(strFilePath);
    if (file.size() > eMaxFileSize)
    {
        QMessageBox::warning(this, 0, "File size too large");
        return;
    }
    if (!file.open(QIODevice::ReadOnly))
    {
        QMessageBox::warning(this, 0, "Can't open file");
        return;
    }
    teEditEx->setPlainText(chbMimeType->isChecked() ?
                               ("data:" + QMimeDatabase().mimeTypeForFile(strFilePath).name() + ";base64," + file.readAll().toBase64()) :
                               file.readAll().toBase64());
    teEditEx->setFocus();
    if (file.error())
    {
        teEditEx->clear();
        QMessageBox::warning(this, 0, file.errorString());
    }
}
