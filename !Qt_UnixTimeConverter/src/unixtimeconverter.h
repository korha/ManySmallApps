#ifndef UNIXTIMECONVERTER_H
#define UNIXTIMECONVERTER_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>
#include <QtWidgets/QTabWidget>
#include <QtGui/QClipboard>
#include <QtCore/QDateTime>
#include <QtCore/QFile>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class UnixTimeConverter : public QWidget
{
    Q_OBJECT
public:
    UnixTimeConverter();
    ~UnixTimeConverter();

private:
    QLineEdit *leUnixUH;
    QCheckBox *chbMSecUH;
    QLineEdit *leFormatUH;
    QLineEdit *leLocalUH;
    QLineEdit *leUTCUH;

    QLineEdit *leDateHU;
    QLineEdit *leFormatHU;
    QLineEdit *leUnixHU;
    QCheckBox *chbMSecHU;

    QTabWidget *twMain;

    QString strLocaleInTime,
    strLocaleInDate;

private slots:
    void slotUnixPaste();
    void slotDecode();
    void slotLocalCopy() const;
    void slotUTCCopy() const;
    void slotDatePaste();
    void slotEncode();
    void slotUnixCopy() const;
    void slotNow();
};

#endif // UNIXTIMECONVERTER_H
