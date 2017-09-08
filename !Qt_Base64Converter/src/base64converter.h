#ifndef BASE64CONVERTER_H
#define BASE64CONVERTER_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTabWidget>
#include <QtGui/QClipboard>
#include <QtCore/QMimeData>
#include <QtCore/QMimeDatabase>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class TextEditEx : public QPlainTextEdit
{
    Q_OBJECT
public:
    explicit TextEditEx(QWidget *parent);

private:
    explicit TextEditEx(const TextEditEx &);
    void operator=(const TextEditEx &);
    virtual void dragEnterEvent(QDragEnterEvent *event);
    virtual void dragMoveEvent(QDragMoveEvent *event);
    virtual void dropEvent(QDropEvent *event);

signals:
    void signalToBase64(const QString &strFilePath);
};

class Base64Converter : public QTabWidget
{
    Q_OBJECT
public:
    Base64Converter();
    ~Base64Converter();

private:
    enum
    {
        eMaxFileSize = 1024*1024
    };
    QPlainTextEdit *tePlain,
    *teBase;
    QCheckBox *chbMimeType;
    TextEditEx *teEditEx;

private slots:
    void slotEncode() const;
    void slotDecode() const;
    void slotPaste() const;
    void slotOpen();
    void slotSave();
    void slotCopy() const;
    void slotToBase64(const QString &strFilePath);
};

#endif // BASE64CONVERTER_H
