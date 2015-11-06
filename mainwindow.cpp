#include <QtDebug>

#include "mainwindow.h"
#include "ui_mainwindow.h"

QVector<QLabel*> tiles;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // Load UI from form design
    ui->setupUi(this);

    // Resize to minimum requirements
    resize(0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnGenerate_clicked()
{
    qDebug() << "Begin map generation";

//    // Destroy any existing tiles
//    for (int i = 0; i < tiles.size(); i++) {
//        tiles[i]->
//    }

    while (!tiles.empty()) {
        ui->glMap->removeWidget(tiles.back());
        delete tiles.back(), tiles.pop_back();
    }


//        delete tiles.back(), tiles.pop_back();

    // Grid size
    int size = -1;

    // Determine size based on user selection
    switch (ui->cboMapSize->currentIndex()) {
    case 0:
        size = 10;
        break;
    case 1:
        size = 15;
        break;
    case 2:
        size = 20;
        break;
    }

    // Should never happen
    if (size == -1) {
        qWarning("Unable to detect size, defaulting to small");
        size = 10;
    }

    // Create tile images and add them to the grid
    for (int i = 1; i < size * size + 1; i++) {
        QPixmap image("/Users/kevin/Documents/github/Traverse/images/groundTile.png");
        QLabel *label = new QLabel();
        label->setPixmap(image);

        tiles.push_back(label);

        ui->glMap->addWidget(label, (i-1) / size, (i-1) % size + 1);
    }


    QPixmap lava("/Users/kevin/Documents/github/Traverse/images/lavaTile.png");

    QLabel *start = new QLabel("start");
    start->setPixmap(lava);
    ui->glMap->addWidget(start, size - 1, 0);

    QLabel *end = new QLabel("end");
    end->setPixmap(lava);
    ui->glMap->addWidget(end, 0, size + 1);

    tiles.push_back(start);
    tiles.push_back(end);

    // Remove any padding
    ui->glMap->setMargin(0);
    ui->glMap->setSpacing(0);

    // Resize to minimum requirements
    qApp->processEvents();
    resize(0, 0);
}

// Randomly generates the game grid
// type argument understands five values
// 0: Normal - even distribution of tile types
// 1: Volcano - 50% lava tiles
// 2: Oasis - 50% water tiles
// 3: Factory - 50% electric tiles
// 4: Forest - 50% ground tiles
QList< QList<int> > generateGrid(int type, int gridSize) {

}
