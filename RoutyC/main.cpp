#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv); // Para inicializar la aplicación Qt
    
    // Para cargar y aplicar la hoja de estilos QSS desde un archivo
    QFile styleFile(":/styles.qss");
    if (!styleFile.exists()) {
        styleFile.setFileName("styles.qss");
    }
    
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        QString styleSheet = stream.readAll();
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }
    
    MainWindow w;
    w.setWindowTitle("RoutyC");
    w.show();
    return QApplication::exec();
}