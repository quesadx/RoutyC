#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <iostream>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

// Button implementations
void MainWindow::on_pbCalculateWithAlgorithm_clicked() {
    std::cout << "pbCalculateWithAlgorithm clicked!" << std::endl;
}

// ComboBox implementations
void MainWindow::on_cbOrigin_currentIndexChanged(int index) {
    std::cout << "cbOrigin changed to index: " << index << std::endl;
}

void MainWindow::on_cbDestination_currentIndexChanged(int index) {
    std::cout << "cbDestination changed to index: " << index << std::endl;
}

void MainWindow::on_cbAlgorithm_currentIndexChanged(int index) {
    std::cout << "cbAlgorithm changed to index: " << index << std::endl;
}

// Menu action implementations
void MainWindow::on_actionSave_triggered() {
    std::cout << "actionSave clicked!" << std::endl;
}

void MainWindow::on_actionLoad_triggered() {
    std::cout << "actionLoad clicked!" << std::endl;
}

void MainWindow::on_actionClearCurrentDisplay_triggered() {
    std::cout << "actionClearCurrentDisplay clicked!" << std::endl;
}

void MainWindow::on_actionCreateRoute_triggered() {
    std::cout << "actionCreateRoute clicked!" << std::endl;
}

void MainWindow::on_actionDeleteRoute_triggered() {
    std::cout << "actionDeleteRoute clicked!" << std::endl;
}

void MainWindow::on_actionCreateStation_triggered() {
    std::cout << "actionCreateStation clicked!" << std::endl;
}

void MainWindow::on_actionDeleteStation_triggered() {
    std::cout << "actionDeleteStation clicked!" << std::endl;
}

void MainWindow::on_actionExportTraversals_triggered() {
    std::cout << "actionExportTraversals clicked!" << std::endl;
}

void MainWindow::on_actionGenerateReport_triggered() {
    std::cout << "actionGenerateReport clicked!" << std::endl;
}


