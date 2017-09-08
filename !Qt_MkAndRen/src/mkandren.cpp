#include "mkandren.h"

//-------------------------------------------------------------------------------------------------
MkAndRen::MkAndRen() : QTabWidget(),
    bCreateNeedUpdate(false),
    bRenameNeedUpdate(false)
{
    this->findChild<QWidget*>("qt_tabwidget_stackedwidget", Qt::FindDirectChildrenOnly)->setAutoFillBackground(true);

    leCreatePath = new QLineEdit(this);
    leCreatePath->setPlaceholderText("Select the folder in which you want to create folders");
    QPushButton *pbCreateBrowse = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), 0, this);
    QHBoxLayout *hblCreatePath = new QHBoxLayout;
    hblCreatePath->addWidget(leCreatePath);
    hblCreatePath->addWidget(pbCreateBrowse);
    teCreate = new QPlainTextEdit(this);
    teCreate->setWordWrapMode(QTextOption::NoWrap);
    QPushButton *pbCreateRun = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Create", this);
    QWidget *wgtCreate = new QWidget(this);
    QVBoxLayout *vblCreate = new QVBoxLayout(wgtCreate);
    vblCreate->setContentsMargins(3, 3, 3, 3);
    vblCreate->setSpacing(2);
    vblCreate->addLayout(hblCreatePath);
    vblCreate->addWidget(teCreate);
    vblCreate->addWidget(pbCreateRun);

    leRenamePath = new QLineEdit(this);
    leRenamePath->setPlaceholderText("Select the folder in which you want to rename files or folders");
    QPushButton *pbRenameBrowse = new QPushButton(style()->standardIcon(QStyle::SP_DialogOpenButton), 0, this);
    QHBoxLayout *hblRenamePath = new QHBoxLayout;
    hblRenamePath->addWidget(leRenamePath);
    hblRenamePath->addWidget(pbRenameBrowse);
    teRename = new QPlainTextEdit(this);
    teRename->setWordWrapMode(QTextOption::NoWrap);

    rbRenameTypeFiles = new QRadioButton("Files", this);
    rbRenameTypeFiles->setChecked(true);
    QRadioButton *rbRenameTypeFolders = new QRadioButton("Folders", this);
    chbRenameSaveExt = new QCheckBox("Keep original extensions", this);
    chbRenameSaveExt->setChecked(true);
    QPushButton *pbRenamePreview = new QPushButton(style()->standardIcon(QStyle::SP_FileDialogContentsView), 0, this);
    pbRenamePreview->setToolTip("Show preview");
    QHBoxLayout *hblRenameType = new QHBoxLayout;
    hblRenameType->addStretch();
    hblRenameType->addWidget(rbRenameTypeFiles);
    hblRenameType->addWidget(rbRenameTypeFolders);
    hblRenameType->addWidget(chbRenameSaveExt);
    hblRenameType->addWidget(pbRenamePreview);
    QPushButton *pbRenameRun = new QPushButton(style()->standardIcon(QStyle::SP_CommandLink), "Rename", this);
    QWidget *wgtRename = new QWidget(this);
    QVBoxLayout *vblRename = new QVBoxLayout(wgtRename);
    vblRename->setContentsMargins(3, 3, 3, 3);
    vblRename->setSpacing(2);
    vblRename->addLayout(hblRenamePath);
    vblRename->addWidget(teRename);
    vblRename->addLayout(hblRenameType);
    vblRename->addWidget(pbRenameRun);

    this->addTab(wgtCreate, "Create");
    this->addTab(wgtRename, "Rename");
    this->setMinimumSize(this->minimumSizeHint());

    tePreview = new QPlainTextEdit(this);
    tePreview->setWindowFlags(Qt::Window);
    tePreview->setWindowTitle(qAppName() + ": Preview");
    tePreview->setWindowModality(Qt::ApplicationModal);
    tePreview->setWordWrapMode(QTextOption::NoWrap);

    //connects
    connect(pbCreateBrowse, SIGNAL(clicked()), this, SLOT(slotCreateBrowse()));
    connect(pbCreateRun, SIGNAL(clicked()), this, SLOT(slotCreateRun()));
    connect(pbRenameBrowse, SIGNAL(clicked()), this, SLOT(slotRenameBrowse()));
    connect(pbRenameRun, SIGNAL(clicked()), this, SLOT(slotRenameRun()));
    connect(teCreate, SIGNAL(textChanged()), this, SLOT(slotCreateChanged()));
    connect(teRename, SIGNAL(textChanged()), this, SLOT(slotRenameChanged()));
    connect(rbRenameTypeFiles, SIGNAL(toggled(bool)), chbRenameSaveExt, SLOT(setEnabled(bool)));
    connect(pbRenamePreview, SIGNAL(clicked()), this, SLOT(slotRenameShowPreview()));

    //arguments
    const QStringList slistArgs = qApp->arguments();
    for (int i = 1; i < slistArgs.size(); ++i)
        if (slistArgs.at(i) == "/tab:rename")
            this->setCurrentIndex(1);
        else if (slistArgs.at(i) == "/type:folders")
            rbRenameTypeFolders->setChecked(true);
        else if (slistArgs.at(i) == "/ext:0")
            chbRenameSaveExt->setChecked(false);

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
MkAndRen::~MkAndRen()
{
    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.open(QIODevice::WriteOnly))
        file.write(this->saveGeometry());
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotCreateBrowse()
{
    const QString strPath = QFileDialog::getExistingDirectory(this, 0, leCreatePath->text());
    if (!strPath.isEmpty())
        leCreatePath->setText(QDir::toNativeSeparators(strPath));
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotCreateRun()
{
    const QString strCreatePath = leCreatePath->text();
    if (strCreatePath.isEmpty())
    {
        leCreatePath->setFocus();
        QMessageBox::warning(this, 0, "Choose folder");
        return;
    }
    const QDir dir(strCreatePath);
    if (!dir.exists() && !dir.mkpath(strCreatePath))
    {
        leCreatePath->setFocus();
        leCreatePath->selectAll();
        QMessageBox::warning(this, 0, "Don't create folder");
        return;
    }
    teCreate->setUpdatesEnabled(false);
    bCreateNeedUpdate = false;
    const QTextCursor txtcurOld = teCreate->textCursor();
    QTextCursor txtcur = txtcurOld;
    txtcur.movePosition(QTextCursor::Start);
    QTextCharFormat txtchformatOk, txtchformatFail;
    txtchformatOk.setForeground(Qt::darkGreen);
    txtchformatFail.setForeground(Qt::red);
    int iOk = 0, iFail = 0;
    do
    {
        txtcur.select(QTextCursor::LineUnderCursor);
        if (txtcur.hasSelection())
        {
            if (dir.mkpath(strCreatePath + '/' + txtcur.selectedText()))
            {
                txtcur.mergeCharFormat(txtchformatOk);
                ++iOk;
            }
            else
            {
                txtcur.mergeCharFormat(txtchformatFail);
                ++iFail;
            }
        }
    } while (txtcur.movePosition(QTextCursor::Down));
    teCreate->setTextCursor(txtcurOld);
    bCreateNeedUpdate = true;
    teCreate->setUpdatesEnabled(true);
    QMessageBox::information(this, 0, "Successfully created " + QString::number(iOk) + " folder(s)\nDon't create " +
                             QString::number(iFail) + " folder(s)");
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotRenameBrowse()
{
    const QString strPath = QFileDialog::getExistingDirectory(this, 0, leRenamePath->text());
    if (!strPath.isEmpty())
        leRenamePath->setText(QDir::toNativeSeparators(strPath));
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotRenameRun()
{
    const QString strRenamePath = leRenamePath->text();
    if (strRenamePath.isEmpty())
    {
        leRenamePath->setFocus();
        QMessageBox::warning(this, 0, "Choose folder");
        return;
    }
    const QDir dir(strRenamePath);
    if (!dir.exists())
    {
        leRenamePath->setFocus();
        leRenamePath->selectAll();
        QMessageBox::warning(this, 0, "Folder not exists");
        return;
    }
    const bool bRenameFiles = rbRenameTypeFiles->isChecked(),
            bRenameSaveExt = chbRenameSaveExt->isChecked();
    teRename->setUpdatesEnabled(false);
    bRenameNeedUpdate = true;
    teRename->textChanged();
    const QTextCursor txtcurOld = teRename->textCursor();
    QTextCursor txtcur = txtcurOld;
    txtcur.movePosition(QTextCursor::Start);
    QTextCharFormat txtchformatOk, txtchformatFail;
    txtchformatOk.setForeground(Qt::darkGreen);
    txtchformatFail.setForeground(Qt::red);
    const QStringList slistOld = dir.entryList(bRenameFiles ? QDir::Files : (QDir::Dirs | QDir::NoDotAndDotDot));
    int iOk = 0, iFail = 0;

    if (!bRenameFiles || !bRenameSaveExt)        //rename folders or files with ignored extensions
        for (int i = 0; i < slistOld.size(); ++i)
        {
            txtcur.select(QTextCursor::LineUnderCursor);
            if (txtcur.hasSelection())
            {
                if (QFile::rename(strRenamePath + '/' + slistOld.at(i), strRenamePath + '/' + txtcur.selectedText()))
                {
                    txtcur.mergeCharFormat(txtchformatOk);
                    ++iOk;
                }
                else
                {
                    txtcur.mergeCharFormat(txtchformatFail);
                    ++iFail;
                }
            }
            if (!txtcur.movePosition(QTextCursor::Down))
                break;
        }
    else        //rename files with save extensions
        for (int i = 0; i < slistOld.size(); ++i)
        {
            txtcur.select(QTextCursor::LineUnderCursor);
            if (txtcur.hasSelection())
            {
                const QString strExt = QFileInfo(slistOld.at(i)).suffix();
                if (strExt.isEmpty())
                {
                    if (QFile::rename(strRenamePath + '/' + slistOld.at(i), strRenamePath + '/' + txtcur.selectedText()))
                    {
                        txtcur.mergeCharFormat(txtchformatOk);
                        ++iOk;
                    }
                    else
                    {
                        txtcur.mergeCharFormat(txtchformatFail);
                        ++iFail;
                    }
                }
                else
                {
                    if (QFile::rename(strRenamePath + '/' + slistOld.at(i), strRenamePath + '/' + txtcur.selectedText() + '.' + strExt))
                    {
                        txtcur.mergeCharFormat(txtchformatOk);
                        ++iOk;
                    }
                    else
                    {
                        txtcur.mergeCharFormat(txtchformatFail);
                        ++iFail;
                    }
                }
            }
            if (!txtcur.movePosition(QTextCursor::Down))
                break;
        }

    teRename->setTextCursor(txtcurOld);
    bRenameNeedUpdate = true;
    teRename->setUpdatesEnabled(true);
    if (bRenameFiles)
        QMessageBox::information(this, 0, "Successfully renamed " + QString::number(iOk) + " file(s)\nDon't rename " + QString::number(iFail) + " file(s)");
    else
        QMessageBox::information(this, 0, "Successfully renamed " + QString::number(iOk) + " folder(s)\nDon't rename " + QString::number(iFail) + " folder(s)");
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotCreateChanged()
{
    if (bCreateNeedUpdate)
    {
        bCreateNeedUpdate = false;
        QTextCursor txtcur = teCreate->textCursor();
        txtcur.select(QTextCursor::Document);
        QTextCharFormat txtchformat;
        txtchformat.setForeground(Qt::black);
        txtcur.mergeCharFormat(txtchformat);
    }
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotRenameChanged()
{
    if (bRenameNeedUpdate)
    {
        bRenameNeedUpdate = false;
        QTextCursor txtcur = teRename->textCursor();
        txtcur.select(QTextCursor::Document);
        QTextCharFormat txtchformat;
        txtchformat.setForeground(Qt::black);
        txtcur.mergeCharFormat(txtchformat);
    }
}

//-------------------------------------------------------------------------------------------------
void MkAndRen::slotRenameShowPreview()
{
    const QString strRenamePath = leRenamePath->text();
    if (strRenamePath.isEmpty())
    {
        leRenamePath->setFocus();
        QMessageBox::warning(this, 0, "Choose folder");
        return;
    }
    const QDir dir(strRenamePath);
    if (!dir.exists())
    {
        leRenamePath->setFocus();
        leRenamePath->selectAll();
        QMessageBox::warning(this, 0, "Folder not exists");
        return;
    }

    const QStringList slistNew = teRename->toPlainText().split('\n');
    for (int i = 0; i < slistNew.size(); ++i)
        if (!slistNew.at(i).isEmpty())
        {
            const bool bRenameFiles = rbRenameTypeFiles->isChecked(),
                    bRenameSaveExt = chbRenameSaveExt->isChecked();
            const QStringList slistOld = dir.entryList(bRenameFiles ? QDir::Files : (QDir::Dirs | QDir::NoDotAndDotDot));
            const int iSize = qMin(slistOld.size(), slistNew.size());
            QString strPreview;
            if (!bRenameFiles || !bRenameSaveExt)        //preview rename folders or files with ignored extensions
                for (int i = 0; i < iSize; ++i)
                    strPreview += slistOld.at(i) + '\n' + slistNew.at(i) + "\n\n";
            else        //preview rename files with save extensions
                for (int i = 0; i < iSize; ++i)
                    if (slistNew.at(i).isEmpty())
                        strPreview += slistOld.at(i) + "\n\n";
                    else
                    {
                        const QString strExt = QFileInfo(slistOld.at(i)).suffix();
                        strPreview += strExt.isEmpty() ?
                                    (slistOld.at(i) + '\n' + slistNew.at(i) + "\n\n") :
                                    (slistOld.at(i) + '\n' + slistNew.at(i) + '.' + strExt + "\n\n");
                    }
            tePreview->setPlainText(strPreview);
            tePreview->show();
            break;
        }
}
