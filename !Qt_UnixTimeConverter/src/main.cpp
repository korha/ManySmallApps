#include "unixtimeconverter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UnixTimeConverter w;
    w.show();
    return a.exec();
}
