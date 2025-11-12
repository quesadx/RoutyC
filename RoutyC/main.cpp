#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qSetMessagePattern("%{message}");
    MainWindow w;
    w.show();
    return QApplication::exec();
}