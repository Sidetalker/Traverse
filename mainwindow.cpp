#include <QtDebug>
#include <algorithm>
#include <cmath>

#include "mainwindow.h"

enum tileTypes {
    meElec,
    meFire,
    meReg,
    meWet,
    ground,
    groundElec,
    groundFire,
    groundReg,
    groundWet,
    lava,
    lavaElec,
    lavaFire,
    lavaReg,
    lavaWet,
    lightning,
    lightningElec,
    lightningFire,
    lightningReg,
    lightningWet,
    water,
    waterElec,
    waterFire,
    waterReg,
    waterWet,
    finish
};
const char *tilePaths[] = {
    "/Users/kevin/Documents/github/Traverse/images/elec.png",
    "/Users/kevin/Documents/github/Traverse/images/fire.png",
    "/Users/kevin/Documents/github/Traverse/images/reg.png",
    "/Users/kevin/Documents/github/Traverse/images/wet.png",
    "/Users/kevin/Documents/github/Traverse/images/groundTile.png",
    "/Users/kevin/Documents/github/Traverse/images/groundTileElec.png",
    "/Users/kevin/Documents/github/Traverse/images/groundTileFire.png",
    "/Users/kevin/Documents/github/Traverse/images/groundTileReg.png",
    "/Users/kevin/Documents/github/Traverse/images/groundTileWet.png",
    "/Users/kevin/Documents/github/Traverse/images/lavaTile.png",
    "/Users/kevin/Documents/github/Traverse/images/lavaTileElec.png",
    "/Users/kevin/Documents/github/Traverse/images/lavaTileFire.png",
    "/Users/kevin/Documents/github/Traverse/images/lavaTileReg.png",
    "/Users/kevin/Documents/github/Traverse/images/lavaTileWet.png",
    "/Users/kevin/Documents/github/Traverse/images/lightningTile.png",
    "/Users/kevin/Documents/github/Traverse/images/lightningTileElec.png",
    "/Users/kevin/Documents/github/Traverse/images/lightningTileFire.png",
    "/Users/kevin/Documents/github/Traverse/images/lightningTileReg.png",
    "/Users/kevin/Documents/github/Traverse/images/lightningTileWet.png",
    "/Users/kevin/Documents/github/Traverse/images/waterTile.png",
    "/Users/kevin/Documents/github/Traverse/images/waterTileElec.png",
    "/Users/kevin/Documents/github/Traverse/images/waterTileFire.png",
    "/Users/kevin/Documents/github/Traverse/images/waterTileReg.png",
    "/Users/kevin/Documents/github/Traverse/images/waterTileWet.png",
    "/Users/kevin/Documents/github/Traverse/images/finish.png"

};

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
    debug("Beginning map generation");

    if (!tiles.empty()) {
        while (!tiles.empty()) {
            ui->glMap->removeWidget(tiles.back());
            delete tiles.back(), tiles.pop_back();
        }

        debug("Existing map destroyed");
    }

    // Grid size
    int size = -1;

    // Determine size based on user selection
    switch (ui->cboMapSize->currentIndex()) {
    case 0:
        size = 10;
        break;
    case 1:
        size = 12;
        break;
    case 2:
        size = 15;
        break;
    }

    // Should never happen
    if (size == -1) {
        qWarning("Unable to detect size, defaulting to small");
        size = 10;
    }

    // Generate grid

    // Create tile images and add them to the grid
    for (int i = 1; i <= size * size; i++) {
        QLabel *label = new QLabel();
        QPixmap image(tilePaths[tileTypes::ground]);
        label->setPixmap(image);

        tiles.push_back(label);

        ui->glMap->addWidget(label, (i-1) / size, (i-1) % size + 1);
    }


    QPixmap startImage(tilePaths[tileTypes::meReg]);
    QPixmap endImage(tilePaths[tileTypes::finish]);

    QLabel *start = new QLabel();
    start->setPixmap(startImage);
    ui->glMap->addWidget(start, size - 1, 0);

    QLabel *end = new QLabel();
    end->setPixmap(endImage);
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
// type argument can take five values
// 0: Normal - even distribution of tile types
// 1: Volcano - 50% lava tiles
// 2: Oasis - 50% water tiles
// 3: Factory - 50% electric tiles
// 4: Forest - 50% ground tiles
std::vector<int> MainWindow::generateGrid(int type, int gridSize) {
    // Determine minimum number of tiles for any map type
    int numTiles = gridSize * gridSize;
    int minTypeCount = std::ceil(numTiles * 0.5 / 3);

    // Initialize tile type vector [lava, water, elec, ground] with min values
    int tiles[4]; std::fill_n(tiles, 4, minTypeCount);

    // Adjust tile counts based on map type
    if (type == 0) {
        std::fill_n(tiles, 4, std::ceil(numTiles / 4.0));
    }
    else {
        tiles[type - 1] = std::ceil(numTiles / 2.0);
    }

    // Fill our return vector with the proper* number of tile types
    std::vector<int> grid;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < tiles[i]; j++) {
            grid.push_back(i);
        }
    }

    // Shuffle the vector with the super handy shuffle function
    std::random_shuffle(grid.begin(), grid.end());

    // When generating tile counts we rounded all numbers up so
    // we might have more than numTiles tiles. Truncate and forget.
    std::vector<int>::const_iterator first = grid.begin();
    std::vector<int>::const_iterator last = grid.begin() + 100;
    std::vector<int> finalGrid(first, last);

    return finalGrid;
}

void MainWindow::debug(QString msg) {
    qDebug() << msg;
    ui->txtConsole->append(msg);
}
