#include "portmapproxy.h"

//-------------------------------------------------------------------------------------------------
PortMapProxy::PortMapProxy() : QObject(),
    bValid(false)
{
    Q_ASSERT(sizeof(quint16) == sizeof(ushort));
    QTextStream cout(stdout);
    QStringList slist = qApp->arguments();
    const QString strFile = (slist.size() > 1) ? slist.at(1) : (qApp->applicationFilePath() + ".cfg");
    QFile file(strFile);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        cout << "Can't open file: " << strFile << endl;
        return;
    }
    slist = QString(file.readAll()).split('\n', QString::SkipEmptyParts);
    if (file.error())
    {
        cout << file.errorString() << ": " << strFile << endl;
        return;
    }
    file.close();

    QVector<TagRule> vectRules;
    TagRule sRule;
    for (int i = 0, iDelim1, iDelim2, iDelim3; i < slist.size(); ++i)
    {
        const QString &strRule = slist.at(i);
        if (strRule.at(0) != ';')
        {
            if (
                    (iDelim1 = strRule.indexOf(':', 7)) >= 0 &&        //[7 = 0.0.0.0]
                    (iDelim2 = strRule.indexOf('|', iDelim1+2)) >= 0 &&        //[2 = :0]
                    (iDelim3 = strRule.indexOf(':', iDelim2+8)) >= 0 &&        //[8 = 0.0.0.0:]
                    QHostAddress().setAddress(sRule.strLocalAddr = strRule.left(iDelim1)) &&
                    (sRule.iLocalPort = strRule.midRef(iDelim1+1, iDelim2-iDelim1-1).toUShort()) &&
                    QHostAddress().setAddress(sRule.strDestAddr = strRule.mid(iDelim2+1, iDelim3-iDelim2-1)) &&
                    (sRule.iDestPort = strRule.midRef(iDelim3+1).toUShort())
                    )
                vectRules.append(sRule);
            else
            {
                cout << "Error: incorrect rule:\n" << strRule << endl;
                return;
            }
        }
    }

    for (int i = 0; i < vectRules.size(); ++i)
    {
        const TagRule &sRule = vectRules.at(i);
        QTcpServer *tcpServer = new QTcpServer(this);
        if (tcpServer->listen(QHostAddress(sRule.strLocalAddr), sRule.iLocalPort))
        {
            tcpServer->setProperty("DAddr", sRule.strDestAddr);
            tcpServer->setProperty("DPort", sRule.iDestPort);
            cout << sRule.strLocalAddr << ':' << sRule.iLocalPort << '|' << sRule.strDestAddr << ':' << sRule.iDestPort << " - OK" << endl;
            bValid = true;
            connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
        }
        else
        {
            cout << sRule.strLocalAddr << ':' << sRule.iLocalPort << '|' << sRule.strDestAddr << ':' << sRule.iDestPort << " - Error: " << tcpServer->errorString() << endl;
            delete tcpServer;
        }
    }

    if (bValid)
    {
#ifdef Q_OS_WIN
        ::SetConsoleCtrlHandler(HandlerRoutine, TRUE);
#endif
    }
    else
        cout << "Error: there are no working rules..." << endl;
}

#ifdef Q_OS_WIN
//-------------------------------------------------------------------------------------------------
BOOL WINAPI PortMapProxy::HandlerRoutine(DWORD)
{
    qApp->quit();
    return TRUE;
}
#endif

//-------------------------------------------------------------------------------------------------
void PortMapProxy::slotNewConnection() const
{
    Q_ASSERT(qobject_cast<QTcpServer*>(this->sender()));
    if (QTcpServer *tcpServer = static_cast<QTcpServer*>(this->sender()))
        while (tcpServer->hasPendingConnections())
        {
            QTcpSocket *tcpSocketClient = tcpServer->nextPendingConnection();
            tcpSocketClient->setProperty("DAddr", tcpServer->property("DAddr").toString());
            tcpSocketClient->setProperty("DPort", tcpServer->property("DPort").toInt());
            connect(tcpSocketClient, SIGNAL(readyRead()), this, SLOT(slotClientRead()));
            connect(tcpSocketClient, SIGNAL(disconnected()), tcpSocketClient, SLOT(deleteLater()));
            connect(tcpSocketClient, SIGNAL(error(QAbstractSocket::SocketError)), tcpSocketClient, SLOT(deleteLater()));
        }
}

//-------------------------------------------------------------------------------------------------
void PortMapProxy::slotClientRead() const
{
    Q_ASSERT(qobject_cast<QTcpSocket*>(this->sender()));
    QTcpSocket *tcpSocketClient = static_cast<QTcpSocket*>(this->sender());
    if (!tcpSocketClient)
        return;
    if (QTcpSocket *tcpSocketServer = tcpSocketClient->findChild<QTcpSocket*>(0, Qt::FindDirectChildrenOnly))
    {
        const QByteArray baRequest = tcpSocketClient->readAll();
        tcpSocketServer->setProperty("Req", baRequest);
        tcpSocketServer->write(baRequest);
    }
    else
    {
        tcpSocketServer = new QTcpSocket(tcpSocketClient);
        tcpSocketServer->setProperty("Req", tcpSocketClient->readAll());
        connect(tcpSocketServer, SIGNAL(connected()), this, SLOT(slotServerWrite()));
        connect(tcpSocketServer, SIGNAL(readyRead()), this, SLOT(slotServerToClient()));
        connect(tcpSocketServer, SIGNAL(disconnected()), this, SLOT(slotClientClose()));
        connect(tcpSocketServer, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(slotClientClose()));
        tcpSocketServer->connectToHost(tcpSocketClient->property("DAddr").toString(), tcpSocketClient->property("DPort").toInt());
    }
}

//-------------------------------------------------------------------------------------------------
void PortMapProxy::slotServerWrite() const
{
    Q_ASSERT(qobject_cast<QTcpSocket*>(this->sender()));
    if (QTcpSocket *tcpSocketServer = static_cast<QTcpSocket*>(this->sender()))
        tcpSocketServer->write(tcpSocketServer->property("Req").toByteArray());
}

//-------------------------------------------------------------------------------------------------
void PortMapProxy::slotServerToClient() const
{
    Q_ASSERT(qobject_cast<QTcpSocket*>(this->sender()));
    if (QTcpSocket *tcpSocketServer = static_cast<QTcpSocket*>(this->sender()))
    {
        Q_ASSERT(qobject_cast<QTcpSocket*>(tcpSocketServer->parent()));
        if (QTcpSocket *tcpSocketClient = static_cast<QTcpSocket*>(tcpSocketServer->parent()))
            tcpSocketClient->write(tcpSocketServer->readAll());
    }
}

//-------------------------------------------------------------------------------------------------
void PortMapProxy::slotClientClose() const
{
    Q_ASSERT(qobject_cast<QTcpSocket*>(this->sender()));
    if (QObject *pObj = this->sender())
    {
        Q_ASSERT(qobject_cast<QTcpSocket*>(pObj->parent()));
        if (QTcpSocket *tcpSocketClient = static_cast<QTcpSocket*>(pObj->parent()))
            tcpSocketClient->disconnectFromHost();
        pObj->deleteLater();
    }
}
