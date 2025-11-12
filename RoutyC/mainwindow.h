#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
        // Button slots
        void on_pbCalculateWithAlgorithm_clicked();

        // Radio button slots
        void on_rbDijkstra_toggled(bool checked);
        void on_rbFloydWarshall_toggled(bool checked);

        // ComboBox slots
        void on_cbOrigin_currentIndexChanged(int index);
        void on_cbDestination_currentIndexChanged(int index);

        // Menu action slots
        void on_actionSave_triggered();
        void on_actionLoad_triggered();
        void on_actionClearCurrentDisplay_triggered();
        void on_actionCreateRoute_triggered();
        void on_actionDeleteRoute_triggered();
        void on_actionCreateStation_triggered();
        void on_actionDeleteStation_triggered();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
