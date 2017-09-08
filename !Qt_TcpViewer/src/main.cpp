#include "tcpviewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TcpViewer w;
    if (w.isValid())
    {
        w.show();
        return a.exec();
    }
    return 0;
}
