#include "base64converter.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Base64Converter w;
    w.show();
    return a.exec();
}
