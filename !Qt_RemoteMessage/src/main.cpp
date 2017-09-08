#include "remotemessage.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RemoteMessage w;
    if (w.isValid())
    {
        w.show();
        return a.exec();
    }
    return 0;
}
