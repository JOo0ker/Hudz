#include "hudz.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Hudz w;
    w.show();
    return a.exec();
}
