#include "portmapproxy.h"

int main(int argc, char *argv[])
{
    {
        QCoreApplication a(argc, argv);
        if (PortMapProxy().isValid())
            return a.exec();
    }
#ifdef Q_OS_WIN
    system("pause");
#endif
    return 0;
}
