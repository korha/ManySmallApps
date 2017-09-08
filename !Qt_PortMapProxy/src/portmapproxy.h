#ifndef PORTMAPPROXY_H
#define PORTMAPPROXY_H

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>
#ifdef Q_OS_WIN
#include <QtCore/qt_windows.h>
#endif

#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

class PortMapProxy : public QObject
{
    Q_OBJECT
public:
    PortMapProxy();
    inline bool isValid() const
    {
        return bValid;
    }

private:
    struct TagRule
    {
        QString strLocalAddr;
        quint16 iLocalPort;
        QString strDestAddr;
        quint16 iDestPort;
    };
    bool bValid;
#ifdef Q_OS_WIN
    static BOOL WINAPI HandlerRoutine(DWORD);
#endif

private slots:
    void slotNewConnection() const;
    void slotClientRead() const;
    void slotServerWrite() const;
    void slotServerToClient() const;
    void slotClientClose() const;
};

#endif // PORTMAPPROXY_H
