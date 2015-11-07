#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ui_mainwindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_btnGenerate_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QLabel*> tiles;
    std::vector<int> tileTypes;

    std::vector<int> generateGrid(int type, int gridSize);
    void debug(QString msg);
};

#endif // MAINWINDOW_H
