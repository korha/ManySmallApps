#include "remotemessage.h"

#define WEB_FONTSIZE "30"
#define WEB_SIZE "200"

RemoteMessage::RemoteMessage() : QLabel(),
    baResponce("<!doctype html>\n"
               "<html>\n"
               "<head>\n"
               "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n"
               "<title>" + qAppName().toUtf8() + "</title>\n"
                                                 "<style type=\"text/css\">\n"
                                                 ".f{float:left;}\n"
                                                 ".b{font-size:" WEB_FONTSIZE "px;width:" WEB_SIZE "px;height:" WEB_SIZE "px;}\n"
                                                                                                                         "</style>\n"
                                                                                                                         "</head>\n"
                                                                                                                         "<body>\n"
                                                                                                                         "<form action=\"command\" method=\"post\" enctype=\"text/plain\">\n"
                                                                                                                         "<div style=\"text-align: center\">\n"
                                                                                                                         "<input name=\"command\" style=\"width:25%\"><br />\n"
                                                                                                                         "<input type=\"submit\" value=\"RUN\">\n"
                                                                                                                         "</div>\n"
                                                                                                                         "</form>\n"
                                                                                                                         "<br /><br />\n"),
    baFavicon(QByteArray::fromRawData(
                  "HTTP/1.1 200 OK\r\n"
                  "Content-Type: image/x-icon\r\n"
                  "Cache-Control: max-age=86400\r\n"
                  "Content-Length: 1150\r\n"
                  "\r\n"
                  "\x00\x00\x01\x00\x01\x00\x10\x10\x00\x00\x01\x00\x20\x00\x68\x04\x00\x00\x16\x00"
                  "\x00\x00\x28\x00\x00\x00\x10\x00\x00\x00\x20\x00\x00\x00\x01\x00\x20\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x23\x23\x23\x1F\x5F\x5F\x5F\xF0\x69\x69"
                  "\x69\xF4\x68\x68\x68\xF4\x68\x66\x65\xF6\x67\x64\x62\xF9\x68\x65\x63\xF9\x6A\x68"
                  "\x68\xF7\x6C\x6C\x6C\xF5\x6D\x6D\x6D\xF5\x6E\x6E\x6E\xF6\x6F\x6F\x6F\xF6\x70\x70"
                  "\x70\xF6\x8C\x8C\x8C\xB8\x00\x00\x00\x00\x00\x00\x00\x00\x12\x12\x12\x97\xFB\xFC"
                  "\xFC\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\xFF\xFF\x71\x71\x71\xF5\x00\x00\x00\x00\x00\x00\x00\x00\x1F\x17"
                  "\x11\xB1\xFF\xFF\xFF\xFF\x30\x2F\x2E\xFF\x19\x1A\x1A\xFF\x1D\x1E\x1E\xFF\x1D\x1D"
                  "\x1E\xFF\x1C\x1C\x1D\xFF\x23\x23\x24\xFF\x2D\x2D\x2D\xFF\x35\x36\x36\xFF\x3E\x3E"
                  "\x3F\xFF\x40\x40\x40\xFF\xFF\xFF\xFF\xFF\x72\x72\x72\xF4\x00\x00\x00\x00\x42\x24"
                  "\x0A\x17\x34\x1D\x0E\xFC\xFF\xFF\xFF\xFF\x32\x31\x30\xFF\x13\x13\x13\xFF\x18\x18"
                  "\x18\xFF\x19\x19\x19\xFF\x1A\x1A\x1A\xFF\x19\x19\x19\xFF\x18\x18\x18\xFF\x21\x21"
                  "\x21\xFF\x2A\x2A\x2A\xFF\x27\x27\x27\xFF\xFF\xFF\xFF\xFF\x73\x73\x73\xF2\x00\x00"
                  "\x00\x00\x57\x2E\x0D\xD2\x46\x2A\x18\xFF\xFF\xFF\xFF\xFF\x2B\x2B\x2A\xFF\x13\x13"
                  "\x13\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\x06\x06\x06\xFF\x06\x06"
                  "\x06\xFF\x07\x07\x07\xFF\x06\x06\x06\xFF\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\x74\x74"
                  "\x74\xF1\x45\x1F\x06\x10\x78\x5A\x43\xFE\x50\x32\x1A\xFF\xFF\xFF\xFF\xFF\x1E\x1D"
                  "\x1B\xFF\x0F\x0F\x0F\xFF\x0B\x0B\x0B\xFF\xFC\xFC\xFC\xFF\xF8\xF8\xF8\xFF\x00\x00"
                  "\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\xFF\xFF"
                  "\xFF\xFF\x75\x75\x75\xF0\x5D\x36\x1A\x77\xA3\x91\x83\xFF\x52\x34\x1B\xFF\xFF\xFF"
                  "\xFF\xFF\x15\x13\x12\xFF\xFB\xFB\xFB\xFF\x2D\x2D\x2D\xFF\x22\x22\x22\xFF\x2D\x2D"
                  "\x2D\xFF\x21\x21\x21\xFF\x09\x09\x09\xFF\x00\x00\x00\xFF\x00\x00\x00\xFF\x00\x00"
                  "\x00\xFF\xFF\xFF\xFF\xFF\x77\x75\x74\xF2\x61\x3A\x1B\xB5\x88\x66\x4A\xFF\x55\x37"
                  "\x1F\xFF\xFF\xFF\xFF\xFF\x0E\x0E\x0D\xFF\x1F\x1F\x1F\xFF\xFD\xFD\xFD\xFF\x27\x27"
                  "\x27\xFF\x37\x37\x37\xFF\x42\x42\x42\xFF\x4E\x4E\x4E\xFF\x51\x51\x51\xFF\x4D\x4D"
                  "\x4D\xFF\x39\x39\x3A\xFF\xFF\xFF\xFF\xFF\x77\x74\x72\xF6\x62\x3B\x1D\xB5\x77\x4A"
                  "\x27\xFF\x57\x3B\x22\xFF\xFF\xFF\xFF\xFF\x03\x02\x01\xFF\xFB\xFB\xFB\xFF\x06\x06"
                  "\x06\xFF\x16\x17\x17\xFF\x25\x25\x25\xFF\x2F\x2F\x2F\xFF\x3A\x3A\x3A\xFF\x45\x45"
                  "\x45\xFF\x4F\x4F\x4F\xFF\x54\x55\x55\xFF\xFF\xFF\xFF\xFF\x7C\x78\x76\xF6\x60\x38"
                  "\x1D\x79\x70\x44\x26\xFF\x57\x3A\x23\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x84\x82"
                  "\x81\xF2\x42\x24\x04\x13\x5E\x36\x18\xFE\x84\x50\x28\xFF\x7C\x71\x68\xFF\x91\x91"
                  "\x8E\xFF\x96\x93\x92\xFF\x97\x94\x93\xFF\x97\x95\x92\xFF\x97\x95\x93\xFF\x8F\x88"
                  "\x82\xFF\x93\x8D\x8A\xFF\x8D\x86\x80\xFF\x86\x7E\x76\xFF\x89\x85\x82\xFF\x71\x6A"
                  "\x63\xFE\xB3\xB4\xB4\x5A\x00\x00\x00\x00\x58\x30\x10\xD6\x73\x47\x22\xFF\xA9\x83"
                  "\x61\xFF\xC1\xA2\x89\xFF\xDD\xCE\xC1\xFF\xEB\xDE\xD3\xFF\xD8\xB6\x96\xFF\xDE\xC0"
                  "\xA5\xFF\xEB\xDB\xCC\xFF\xED\xE2\xD9\xFF\xE3\xD7\xCE\xFF\xDC\xD3\xCC\xFF\xBA\xAB"
                  "\x9F\xFF\x5A\x2F\x0D\xCC\x00\x00\x00\x00\x00\x00\x00\x00\x4C\x23\x0D\x1A\x58\x32"
                  "\x13\xFA\x94\x6D\x4D\xFF\xD0\xBA\xA8\xFF\xDB\xCE\xC3\xFF\xDD\xCD\xBE\xFF\xE6\xDB"
                  "\xD1\xFF\xEA\xE1\xD7\xFF\xE1\xD1\xC3\xFF\xE7\xE2\xDD\xFF\xE7\xE1\xDC\xFF\xDC\xD6"
                  "\xD1\xFF\x76\x56\x3D\xFA\x59\x2F\x0E\x12\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x4E\x2C\x06\x53\x56\x30\x10\xFA\xAE\x93\x7E\xFF\xF0\xE8\xE2\xFF\xF3\xEF"
                  "\xEB\xFF\xF0\xEC\xE8\xFF\xF3\xF1\xEE\xFF\xF4\xF2\xF1\xFF\xF8\xF7\xF7\xFF\xEC\xE8"
                  "\xE7\xFF\x82\x67\x51\xFA\x53\x2B\x07\x48\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x4D\x23\x0D\x1A\x51\x2B\x09\xD5\x7C\x5F"
                  "\x48\xFE\xD4\xC8\xBD\xFF\xF4\xF1\xED\xFF\xFF\xFF\xFF\xFF\xEA\xE6\xE3\xFF\xB9\xAC"
                  "\xA0\xFE\x65\x40\x23\xD3\x4B\x2B\x0E\x14\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x4E\x29\x0A\x12\x2E\x1B\x0D\xA6\x66\x40\x20\xB5\x64\x3C\x1E\xB2\x2C\x1B"
                  "\x0E\xA2\x4E\x2A\x0B\x0E\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\xC0\x00\x00\x00\xC0\x00\x00\x00\xC0\x00\x00\x00\x80\x00"
                  "\x00\x00\x80\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"
                  "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x80\x01\x00\x00\x80\x01\x00\x00\xC0\x03"
                  "\x00\x00\xE0\x07\x00\x00\xF8\x1F\x00", 1249)),
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

    //settings
    int iRules = 0;
    QFile file(qApp->applicationDirPath() + '/' + qAppName() + ".cfg");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        const QStringList slistRules = QString(file.readAll()).split('\n', QString::SkipEmptyParts);
        if (!file.error())
            for (int i = 0, iDelim; i < slistRules.size(); ++i)
            {
                iDelim = slistRules.at(i).indexOf('|');
                if (iDelim > 0 && slistRules.at(i).lastIndexOf('|') == iDelim)
                {
                    baResponce += "<form action=\"" + QByteArray::number(iRules++) + "\" method=\"post\" enctype=\"text/plain\" class=\"f\">\n"
                                                                                     "<div><input type=\"submit\" value=\"" + slistRules.at(i).left(iDelim).toHtmlEscaped() + "\" class=\"b\"></div>\n"
                                                                                                                                                                              "</form>\n";
                    slistEntries.append(slistRules.at(i).mid(iDelim+1));
                }
            }
        file.close();
    }
    baResponce += "</body>\n</html>\n";
    baResponce.prepend("HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/html\r\n"
                       "Cache-Control: no-cache\r\n"
                       "Content-Length: " + QByteArray::number(baResponce.length())  + "\r\n\r\n");

    this->setAlignment(Qt::AlignCenter);
    this->setText("Server: " + hAddr.toString() + ':' + QString::number(tcpServer->serverPort()) +
                  "\nRules: " + QString::number(iRules));
    this->setFixedSize(250, 50);

    //connects
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));
}

//-------------------------------------------------------------------------------------------------
void RemoteMessage::slotNewConnection() const
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
void RemoteMessage::slotClientRead() const
{
    Q_ASSERT(qobject_cast<QTcpSocket*>(this->sender()));
    QTcpSocket *tcpSocket = static_cast<QTcpSocket*>(sender());
    if (!tcpSocket)
        return;
    QByteArray baRequest = tcpSocket->readAll();
    if (baRequest.startsWith("POST /command "))
    {
        int iFind = baRequest.indexOf("\ncommand=", 14);        //[14 = "POST /command "]
        if (iFind >= 0)
        {
            iFind += 9;
            for (int i = iFind; i < baRequest.size(); ++i)
                if (baRequest.at(i) == '\r' || baRequest.at(i) == '\n')
                {
                    baRequest.truncate(i);
                    break;
                }
            QProcess::startDetached(baRequest.mid(iFind));
        }
    }
    else if (baRequest.startsWith("POST /"))
    {
        int iTemp = baRequest.indexOf(' ', 6);        //[6 = "POST /"]
        if (iTemp > 0)
        {
            bool bOk;
            iTemp = baRequest.mid(6, iTemp-6).toInt(&bOk);
            if (bOk && iTemp < slistEntries.size())
                QProcess::startDetached(slistEntries.at(iTemp));
        }
    }
    else if (baRequest.startsWith("GET /favicon.ico "))
    {
        tcpSocket->write(baFavicon);
        return;
    }
    else if (baRequest.startsWith("GET /screen/"))
    {
        const int iDelim = baRequest.indexOf(' ', 12);        //[12 = "GET /screen/"]
        if (iDelim > 0)
        {
            const QPixmap pixmap = qApp->primaryScreen()->grabWindow(qApp->desktop()->winId());
            Q_ASSERT(!pixmap.isNull());
            if (!pixmap.isNull())
            {
                QByteArray baImg;
                QBuffer buffer(&baImg);
                if (buffer.open(QIODevice::WriteOnly))
                {
                    const int iQuality = baRequest.mid(12, iDelim-12).toInt();        //[12 = "GET /screen/"]
                    if (iQuality > 0 && iQuality <= 100)
                    {
                        if (pixmap.save(&buffer, "JPG", iQuality))
                            tcpSocket->write("HTTP/1.1 200 OK\r\n"
                                             "Content-Type: image/jpeg\r\n"
                                             "Cache-Control: no-cache\r\n"
                                             "Content-Length: " + QByteArray::number(baImg.length()) + "\r\n\r\n" +
                                             baImg);
                    }
                    else if (pixmap.save(&buffer, "PNG", 0))
                        tcpSocket->write("HTTP/1.1 200 OK\r\n"
                                         "Content-Type: image/png\r\n"
                                         "Cache-Control: no-cache\r\n"
                                         "Content-Length: " + QByteArray::number(baImg.length()) + "\r\n\r\n" +
                                         baImg);
                    buffer.close();
                }
            }
        }
    }
    else if (baRequest.startsWith("command="))
    {
        for (int i = 8; i < baRequest.size(); ++i)
            if (baRequest.at(i) == '\r' || baRequest.at(i) == '\n')
            {
                baRequest.truncate(i);
                break;
            }
        Q_ASSERT(baRequest.indexOf('\r') < 0 && baRequest.indexOf('\n') < 0);
        QProcess::startDetached(baRequest.mid(8));
    }
    tcpSocket->write(baResponce);
}
