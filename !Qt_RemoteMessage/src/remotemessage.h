#ifndef REMOTEMESSAGE_H
#define REMOTEMESSAGE_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QDesktopWidget>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtGui/QScreen>
#include <QtCore/QBuffer>
#include <QtCore/QFile>
#include <QtCore/QProcess>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class RemoteMessage : public QLabel
{
    Q_OBJECT
public:
    RemoteMessage();
    inline bool isValid() const
    {
        return bValid;
    }

private:
    QTcpServer *tcpServer;
    QByteArray baResponce;
    const QByteArray baFavicon;
    QStringList slistEntries;
    bool bValid;

private slots:
    void slotNewConnection() const;
    void slotClientRead() const;
};

#endif // REMOTEMESSAGE_H
