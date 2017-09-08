#include "sattoplaylist.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SatToPlaylist w;
    w.show();
    return a.exec();
}
