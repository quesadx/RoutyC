#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

// Button implementations
void MainWindow::on_pbCalculateWithAlgorithm_clicked() {
    qDebug() << "pbCalculateWithAlgorithm clicked!";
}

// Radio button implementations
void MainWindow::on_rbDijkstra_toggled(bool checked) {
    if (checked) {
        qDebug() << "rbDijkstra clicked!";
    }
}

void MainWindow::on_rbFloydWarshall_toggled(bool checked) {
    if (checked) {
        qDebug() << "rbFloydWarshall clicked!";
    }
}

// ComboBox implementations
void MainWindow::on_cbOrigin_currentIndexChanged(int index) {
    qDebug() << "cbOrigin clicked!";
}

void MainWindow::on_cbDestination_currentIndexChanged(int index) {
    qDebug() << "cbDestination clicked!";
}

// Menu action implementations
void MainWindow::on_actionSave_triggered() {
    qDebug() << "actionSave clicked!";
}

void MainWindow::on_actionLoad_triggered() {
    qDebug() << "actionLoad clicked!";
}

void MainWindow::on_actionClearCurrentDisplay_triggered() {
    qDebug() << "actionClearCurrentDisplay clicked!";
}

void MainWindow::on_actionCreateRoute_triggered() {
    qDebug() << "actionCreateRoute clicked!";
}

void MainWindow::on_actionDeleteRoute_triggered() {
    qDebug() << "actionDeleteRoute clicked!";
}

void MainWindow::on_actionCreateStation_triggered() {
    qDebug() << "actionCreateStation clicked!";
}

void MainWindow::on_actionDeleteStation_triggered() {
    qDebug() << "actionDeleteStation clicked!";
}

