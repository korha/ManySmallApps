#include "tcpviewer.h"

//-------------------------------------------------------------------------------------------------
TcpViewer::TcpViewer() : QWidget(),
    baRespBeg("<!doctype html>\n"
              "<html>\n"
              "<head>\n"
              "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
              "<title>" + qAppName().toUtf8() + "</title>\n"
                                                "</head>\n"
                                                "<body>\n"
                                                "OK - "),
    baFavicon(QByteArray::fromRawData(
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: image/x-icon\r\n"
                  "Cache-Control: max-age=86400\r\n"
                  "Content-Length: 1150\r\n"
                  "\r\n"
                  "\x00\x00\x01\x00\x01\x00\x10\x10\x00\x00\x01\x00\x20\x00\x68\x04\x00\x00\x16\x00"
                  "\x00\x00\x28\x00\x00\x00\x10\x00\x00\x00\x20\x00\x00\x00\x01\x00\x20\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x44\x44\x01\x40\x44\x44\x0B\x0C\x0C"
                  "\x0C\x70\x07\x08\x08\x6D\x25\x27\x28\xDD\x21\x21\x21\xFD\x25\x25\x25\xFE\x27\x27"
                  "\x27\xFE\x1E\x1E\x1E\xFD\x27\x28\x28\xEB\x10\x10\x10\x77\x3F\x43\x43\x0B\x40\x44"
                  "\x44\x14\x40\x44\x44\x0C\x40\x44\x44\x04\x40\x44\x44\x05\x00\x00\x00\x00\x40\x44"
                  "\x44\x02\x0A\x0B\x0B\xFE\x2A\x2B\x2C\xFE\x94\x94\x94\xFF\xF4\xF4\xF4\xFF\xE5\xE3"
                  "\xD5\xFF\x9B\x8E\x40\xFF\x98\x81\x12\xFF\x7B\x71\x35\xFF\x38\x38\x34\xFF\x26\x27"
                  "\x29\xE4\x30\x34\x34\x1F\x40\x44\x44\x14\x40\x44\x44\x02\x40\x44\x44\x12\x40\x44"
                  "\x44\x08\x00\x00\x00\x00\x17\x18\x18\xFB\xE2\xE3\xE3\xFF\xE8\xE9\xE9\xFF\xEB\xEC"
                  "\xEC\xFF\x97\x88\x3B\xFF\xA8\x8A\x2E\xFF\x80\x5F\x23\xFF\xAD\x8D\x23\xFF\xA0\x91"
                  "\x44\xFF\x5E\x5E\x5F\xFF\x27\x28\x29\xDD\x3C\x3F\x3F\x0C\xAC\x9F\x96\xFF\x90\x85"
                  "\x7E\xFF\x90\x85\x7E\xFF\x90\x85\x7E\xFF\x4B\x47\x45\xFF\x85\x85\x85\xFF\xEB\xEC"
                  "\xEC\xFF\xE8\xE9\xE9\xFF\x97\x85\x35\xFF\xB1\x9E\x64\xFF\x23\x09\x00\xFF\x7C\x5B"
                  "\x1E\xFF\x95\x83\x1E\xFF\xF8\xF8\xF8\xFF\x2E\x2E\x2E\xFE\x0D\x0D\x0D\x89\xCD\xBE"
                  "\xB4\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xE9\xE9\xE9\xFF\x16\x16"
                  "\x16\xFF\x85\x85\x85\xFF\xE5\xE5\xE5\xFF\x98\x8B\x53\xFF\xB7\xA4\x6C\xFF\x78\x57"
                  "\x20\xFF\xA9\x88\x29\xFF\x9B\x8C\x3F\xFF\xFF\xFF\xFF\xFF\xC8\xC8\xC8\xFF\x20\x20"
                  "\x20\xFB\xCF\xBF\xB6\xFF\xFE\xFE\xFE\xFF\xF3\xE1\xD6\xFF\xF3\xE1\xD6\xFF\xF3\xE1"
                  "\xD6\xFF\xB1\xA4\x9D\xFF\x0E\x0E\x0E\xFF\x48\x48\x48\xFF\x90\x8F\x89\xFF\x81\x75"
                  "\x41\xFF\x7E\x6A\x1A\xFF\x95\x87\x41\xFF\xD7\xD7\xCB\xFF\xD1\xD1\xD1\xFF\x70\x70"
                  "\x70\xFE\x1E\x1E\x1E\xD7\xD0\xC0\xB7\xFF\xF8\xF9\xF9\xFF\xF2\xE1\xD6\xFF\xF4\xE2"
                  "\xD7\xFF\xF4\xE2\xD7\xFF\xF4\xE2\xD7\xFF\xB4\xA8\xA0\xFF\x19\x19\x18\xFF\x28\x28"
                  "\x28\xFF\x51\x51\x51\xFF\x6B\x6C\x6C\xFF\x83\x85\x85\xFF\x6E\x6E\x6F\xFF\x20\x20"
                  "\x20\xFF\x22\x22\x22\xC3\x2F\x31\x31\x13\xD0\xC1\xB9\xFF\xF7\xF8\xF8\xFF\xBB\xAE"
                  "\xA7\xFF\xC2\xB4\xAD\xFF\xC4\xB6\xAE\xFF\xC4\xB6\xAE\xFF\xC4\xB6\xAE\xFF\xB7\xAA"
                  "\xA3\xFF\x65\x60\x5C\xFF\x27\x27\x27\xFF\x19\x1A\x1A\xFF\x1D\x1D\x1E\xFF\x21\x21"
                  "\x21\xE9\x27\x27\x27\x6C\x00\x00\x00\x00\x40\x44\x44\x03\xD1\xC2\xBA\xFF\xF8\xF9"
                  "\xF9\xFF\xE7\xD7\xCE\xFF\xE3\xD3\xCA\xFF\xF4\xE2\xD8\xFF\xF6\xE4\xDA\xFF\xF6\xE4"
                  "\xDA\xFF\xF6\xE4\xDA\xFF\xF6\xE4\xDA\xFF\xFF\xFF\xFF\xFF\x93\x88\x82\xFF\x40\x44"
                  "\x44\x21\x40\x44\x44\x38\x40\x44\x44\x21\x40\x44\x44\x02\x00\x00\x00\x00\xD2\xC3"
                  "\xBB\xFF\xFE\xFE\xFE\xFF\xBD\xAF\xA9\xFF\xB6\xA9\xA3\xFF\xB8\xAB\xA5\xFF\xC4\xB5"
                  "\xAF\xFF\xC6\xB7\xB0\xFF\xC6\xB7\xB0\xFF\xC6\xB7\xB0\xFF\xFF\xFF\xFF\xFF\x94\x89"
                  "\x84\xFF\x40\x44\x44\x03\x40\x44\x44\x21\x40\x44\x44\x38\x40\x44\x44\x19\x40\x44"
                  "\x44\x01\xD3\xC4\xBC\xFF\xFF\xFF\xFF\xFF\xF6\xE5\xDC\xFF\xE5\xD5\xCD\xFF\xE2\xD3"
                  "\xCB\xFF\xE5\xD5\xCD\xFF\xF6\xE4\xDB\xFF\xF8\xE6\xDD\xFF\xF8\xE6\xDD\xFF\xFF\xFF"
                  "\xFF\xFF\x95\x8A\x85\xFF\x00\x00\x00\x00\x40\x44\x44\x03\x40\x44\x44\x21\x40\x44"
                  "\x44\x2B\x40\x44\x44\x0C\xD5\xC5\xBE\xFF\xFF\xFF\xFF\xFF\xC8\xBA\xB2\xFF\xC6\xB8"
                  "\xB1\xFF\xBA\xAE\xA7\xFF\xB8\xAC\xA5\xFF\xBA\xAE\xA7\xFF\xF8\xE6\xDD\xFF\xDF\xCF"
                  "\xC7\xFF\xC8\xBA\xB2\xFF\x78\x6F\x6B\xFF\x40\x44\x44\x08\x00\x00\x00\x00\x40\x44"
                  "\x44\x03\x40\x44\x44\x18\x40\x44\x44\x14\xD6\xC6\xBF\xFF\xFF\xFF\xFF\xFF\xFB\xE9"
                  "\xE0\xFF\xFB\xE9\xE0\xFF\xF9\xE7\xDE\xFF\xE8\xD8\xD0\xFF\xE5\xD5\xCD\xFF\xA4\x99"
                  "\x93\xFF\x7C\x73\x6F\xFF\x7D\x74\x70\xFF\x97\x8C\x86\xFF\x40\x44\x44\x1D\x40\x44"
                  "\x44\x08\x00\x00\x00\x00\x40\x44\x44\x02\x40\x44\x44\x0B\xD6\xC7\xC0\xFF\xFF\xFF"
                  "\xFF\xFF\xFC\xEA\xE2\xFF\xFC\xEA\xE2\xFF\xFC\xEA\xE2\xFF\xFA\xE9\xE1\xFF\xEF\xDF"
                  "\xD7\xFF\xB0\xA5\x9F\xFF\xF0\xDF\xD8\xFF\xF6\xE5\xDB\xFF\xA7\x9A\x92\xB7\x40\x44"
                  "\x44\x08\x40\x44\x44\x1D\x40\x44\x44\x08\x00\x00\x00\x00\x40\x44\x44\x01\xD7\xC8"
                  "\xC1\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFE\xFE\xFE\xFF\xCF\xC1\xBB\xFF\xF5\xE4\xDD\xFF\xA7\x9D\x96\xBA\x40\x44"
                  "\x44\x01\x00\x00\x00\x00\x40\x44\x44\x06\x40\x44\x44\x12\x40\x44\x44\x05\x00\x00"
                  "\x00\x00\xD8\xC9\xC3\xFF\xD8\xC9\xC3\xFF\xD8\xC9\xC3\xFF\xD8\xC9\xC3\xFF\xD8\xC9"
                  "\xC3\xFF\xD8\xC9\xC3\xFF\xD8\xC9\xC3\xFF\xCB\xBD\xB7\xFF\xB0\xA4\x9F\xB7\x40\x44"
                  "\x44\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x40\x44\x44\x02\x40\x44"
                  "\x44\x04\x00\x00\x00\x00\xC0\x00\x00\x00\x20\x00\x00\x00\x10\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x02\x00\x00\x00\x01"
                  "\x00\x00\x00\x00\x00\x00\x00\x10\x00\x00\x00\x08\x00\x00\x00\x04\x00\x00\x00\x02"
                  "\x00\x00\x00\x11\x00\x00\x00\x39\x00", 1249)),
    bValid(true)
{
#ifdef __MINGW32__
#pragma GCC diagnostic warning "-Wpedantic"
#endif

    Q_ASSERT(sizeof(quint16) == sizeof(ushort));

    QHostAddress hAddr(QHostAddress::AnyIPv4);
    quint16 iPort = 0;

    //arguments
    const QStringList slistArgs = qApp->arguments();
    if (slistArgs.size() > 1)
    {
        const QString &strArg = slistArgs.at(1);
        const int iLeft = strArg.indexOf(':', 7);        //[7 = 0.0.0.0:]
        if (iLeft < 0)
        {
            bValid = false;
            QMessageBox::critical(0, 0, "Incorrect parameter");
            return;
        }

        bool bOk;
        iPort = strArg.midRef(iLeft+1).toUShort(&bOk);
        if (!(bOk && hAddr.setAddress(strArg.left(iLeft))))
        {
            bValid = false;
            QMessageBox::critical(0, 0, "Incorrect parameter");
            return;
        }
    }

    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen(hAddr, iPort))
    {
        bValid = false;
        QMessageBox::critical(0, 0, "Don't run server: " + hAddr.toString() + ':' + QString::number(iPort) + '\n' + tcpServer->errorString());
        return;
    }

    QLabel *lblServer = new QLabel("Server: " + hAddr.toString() + ':' + QString::number(tcpServer->serverPort()));
    QPushButton *pbClear = new QPushButton(style()->standardIcon(QStyle::SP_DialogResetButton), "Clean", this);
    QHBoxLayout *hblTop = new QHBoxLayout;
    hblTop->setContentsMargins(3, 3, 3, 3);
    hblTop->addWidget(lblServer);
    hblTop->addWidget(pbClear, 0, Qt::AlignRight);

    teMain = new QPlainTextEdit(this);
    teMain->setWordWrapMode(QTextOption::NoWrap);
    QVBoxLayout *vblMain = new QVBoxLayout(this);
    vblMain->setContentsMargins(0, 0, 0, 0);
    vblMain->setSpacing(0);
    vblMain->addLayout(hblTop);
    vblMain->addWidget(teMain);

    //connects
    connect(pbClear, SIGNAL(clicked()), teMain, SLOT(clear()));
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    QFile file(qApp->applicationFilePath() + ".geo");
    if (file.size() == 50 && file.open(QIODevice::ReadOnly))
    {
        const QByteArray baGeo = file.readAll();
        if (!file.error())
            this->restoreGeometry(baGeo);
    }
}

//-------------------------------------------------------------------------------------------------
TcpViewer::~TcpViewer()
{
    if (bValid)
    {
        QFile file(qApp->applicationFilePath() + ".geo");
        if (file.open(QIODevice::WriteOnly))
            file.write(this->saveGeometry());
    }
}

//-------------------------------------------------------------------------------------------------
void TcpViewer::slotNewConnection() const
{
    while (tcpServer->hasPendingConnections())
    {
        QTcpSocket *tcpSocket = tcpServer->nextPendingConnection();
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(slotClientRead()));
        connect(tcpSocket, SIGNAL(disconnected()), tcpSocket, SLOT(deleteLater()));
        connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), tcpSocket, SLOT(deleteLater()));
    }
}

//-------------------------------------------------------------------------------------------------
void TcpViewer::slotClientRead() const
{
    Q_ASSERT(qobject_cast<QTcpSocket*>(this->sender()));
    if (QTcpSocket *tcpSocket = static_cast<QTcpSocket*>(this->sender()))
    {
        const QString strPeerAddress = tcpSocket->peerAddress().toString();
        const quint16 iPort = tcpSocket->peerPort();
        const QByteArray baRequest = tcpSocket->readAll();

        teMain->appendHtml("<b>" + QString("===== " + strPeerAddress + ':' + QString::number(iPort) +
                                           " ===== " + QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss") + ' ').leftJustified(80, '=') + "</b>");
        QScrollBar *scrbar = teMain->verticalScrollBar();
        const bool bScrollDown = scrbar->value() == scrbar->maximum();
        QTextCursor txtcur = teMain->textCursor();
        txtcur.movePosition(QTextCursor::End);
        txtcur.insertText('\n' + baRequest, QTextCharFormat());
        if (bScrollDown)
            scrbar->setValue(scrbar->maximum());

        if (baRequest.startsWith("GET /favicon.ico HTTP/"))
            tcpSocket->write(baFavicon);
        else
        {
            const QByteArray baResponse = baRespBeg + strPeerAddress.toUtf8() + ':' + QByteArray::number(iPort) + "\n"
                                                                                                                  "</body>\n"
                                                                                                                  "</html>\n";
            tcpSocket->write("HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/html\r\n"
                             "Cache-Control: no-cache\r\n"
                             "Content-Length: " + QByteArray::number(baResponse.length()) + "\r\n\r\n" + baResponse);
        }
        tcpSocket->disconnectFromHost();
    }
}
