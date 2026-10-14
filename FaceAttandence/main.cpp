#include "faceattandance.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FaceAttandance w;
    w.show();
    return a.exec();
}
