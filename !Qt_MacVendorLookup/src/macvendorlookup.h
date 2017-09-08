#ifndef MACVENDORLOOKUP_H
#define MACVENDORLOOKUP_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtGui/QClipboard>
#include <QtCore/QFile>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class MacVendorLookup : public QWidget
{
    Q_OBJECT
public:
    MacVendorLookup();
    ~MacVendorLookup();

private:
    QLineEdit *leRawMac;
    QLabel *lblNotes;
    QMap<uint, QString> map;

private slots:
    void slotEdited(QString strMac);
    void slotPaste();
};

#endif // MACVENDORLOOKUP_H
