#ifndef TCPVIEWER_H
#define TCPVIEWER_H

#include <QtWidgets/QApplication>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QStyle>
#include <QtCore/QDateTime>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

class TcpViewer : public QWidget
{
    Q_OBJECT
public:
    TcpViewer();
    ~TcpViewer();
    inline bool isValid() const
    {
        return bValid;
    }

private:
    QPlainTextEdit *teMain;
    QTcpServer *tcpServer;
    const QByteArray baRespBeg,
    baFavicon;
    bool bValid;
    
private slots:
    void slotNewConnection() const;
    void slotClientRead() const;
};

#endif // TCPVIEWER_H
