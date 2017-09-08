#include "beeptest.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BeepTest w;
    w.show();
    return a.exec();
}
