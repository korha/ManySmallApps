#ifndef SATTOPLAYLIST_H
#define SATTOPLAYLIST_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStyle>

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class SatToPlaylist : public QWidget
{
    Q_OBJECT
public:
    SatToPlaylist();
    ~SatToPlaylist();

private:
    QLineEdit *leAddress;
    QPushButton *pbExportM3u,
    *pbExportMpcpl;
    QByteArray baM3u,
    baMpcpl;

private slots:
    void slotOpen();
    void slotExportM3u();
    void slotExportMpcpl();
};

#endif // SATTOPLAYLIST_H
