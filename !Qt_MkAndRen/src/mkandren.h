#ifndef MKANDREN_H
#define MKANDREN_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTabWidget>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class MkAndRen : public QTabWidget
{
    Q_OBJECT
public:
    MkAndRen();
    ~MkAndRen();

private:
    QLineEdit *leCreatePath;
    QPlainTextEdit *teCreate;
    QLineEdit *leRenamePath;
    QPlainTextEdit *teRename;
    QRadioButton *rbRenameTypeFiles;
    QCheckBox *chbRenameSaveExt;
    QPlainTextEdit *tePreview;
    bool bCreateNeedUpdate,
    bRenameNeedUpdate;

private slots:
    void slotCreateBrowse();
    void slotCreateRun();
    void slotRenameBrowse();
    void slotRenameRun();
    void slotCreateChanged();
    void slotRenameChanged();
    void slotRenameShowPreview();
};

#endif // MKANDREN_H
