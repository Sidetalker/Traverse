#include <QtDebug>
#include <QLabel>
#include <QGridLayout>
#include <QPixmap>

#include "gamewindow.h"
#include "ui_gamewindow.h"

GameWindow::GameWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameWindow)
{
    // Configure from form
    ui->setupUi(this);
    // Create grid layout for game tiles
//    QGridLayout *layout = new QGridLayout;

    int size = 10;


    for (int i = 1; i < size * size + 1; i++) {
        QPixmap image("/Users/kevin/Documents/github/Traverse/images/groundTile.png");
        QLabel *label = new QLabel();
        label->setPixmap(image);
        ui->gridLayout->addWidget(label, (i-1) / size, (i-1) % size + 1);
    }


    QPixmap lava("/Users/kevin/Documents/github/Traverse/images/lavaTile.png");
    QPixmap reg("/Users/kevin/Documents/github/Traverse/images/reg.png");

    QLabel *start = new QLabel("start");
    start->setPixmap(lava);
    ui->gridLayout->addWidget(start, size - 1, 0);

    QLabel *startMe = new QLabel("startMe");
    start->setPixmap(reg);
    ui->gridLayout->addWidget(start, size - 1, 0);

    QLabel *end = new QLabel("end");
    end->setPixmap(lava);
    ui->gridLayout->addWidget(end, 0, size + 1);


    ui->gridLayout->setMargin(0);
    ui->gridLayout->setSpacing(0);


    qDebug() << "Test2";
}

GameWindow::~GameWindow()
{
    delete ui;
}
