#include "bitsetsmainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    BitSetsMainWindow w;
    w.show();

    return a.exec();
}
