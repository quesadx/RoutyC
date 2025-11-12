#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    qSetMessagePattern("%{message}");
    
    QFile styleFile(":/obsidian_theme.qss");
    if (!styleFile.exists()) {
        styleFile.setFileName("obsidian_theme.qss");
    }
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    MainWindow w;
    w.setWindowTitle("RoutyC - Ciudad de la mancha");
    w.show();
    return QApplication::exec();
}