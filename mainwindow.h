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
    void on_btnClearConsole_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QLabel*> tiles;
    std::vector<int> tileTypes;
    int playerLoc[2];

    std::vector<int> generateGrid(int type, int gridSize);
    QString getGridString(std::vector<int> grid);
    void debug(QString msg);
    void keyPressEvent(QKeyEvent *);
    bool move(int direction);
};

#endif // MAINWINDOW_H
