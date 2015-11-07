#include <QtDebug>
#include <QKeyEvent>

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

    // Prefer focus to main window unless explicity denied
    this->setFocusPolicy(Qt::StrongFocus);

    // Resize to minimum requirements
    resize(0, 0);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Handle keypress events
void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();

    if (key == Qt::Key_Left || key == Qt::Key_A) {
        debug("Left pressed");
        move(0);
    }
    else if (key == Qt::Key_Right || key == Qt::Key_D) {
        debug("Right pressed");
        move(1);
    }
    else if (key == Qt::Key_Up || key == Qt::Key_W) {
        debug("Up pressed");
        move(2);
    }
    else if (key == Qt::Key_Down || key == Qt::Key_S) {
        debug("Down pressed");
        move(3);
    }
    else {
        // Ignore keypress
        debug("Key ignored");
    }
}

// Attempt to move in one of four directions
// 0: Left
// 1: Right
// 2: Up
// 3: Down
bool MainWindow::move(int direction) {
    // We're at the starting tile - we can only move right
    if (playerLoc[0] == -1) {
        if (direction != 1) {
            return false
        }
    }

    switch (direction) {
    case 0:

    }
}

// Generate a new map
void MainWindow::on_btnGenerate_clicked()
{
    ui->txtConsole->clear();

    debug("Beginning map generation");

    if (!tiles.empty()) {
        while (!tiles.empty()) {
            ui->glMap->removeWidget(tiles.back());
            delete tiles.back(), tiles.pop_back();
        }

        debug("Existing map destroyed");
    }

    // Grid size/type
    int gridSize = -1;
    int gridType = ui->cboMapType->currentIndex();

    // Determine size based on user selection
    switch (ui->cboMapSize->currentIndex()) {
    case 0:
        gridSize = 10;
        break;
    case 1:
        gridSize = 12;
        break;
    case 2:
        gridSize = 15;
        break;
    }

    // Should never happen
    if (gridSize == -1) {
        qWarning("Unable to detect size, defaulting to small");
        gridSize = 10;
    }

    // Generate grid
    tileTypes = generateGrid(gridType, gridSize);

    debug(QString::number(tileTypes.size()) + " tiles generated");

    // Get representative string for generated grid
    ui->txtMapCode->setText(getGridString(tileTypes));

    debug("Map import string generated");

    // Create tile images and add them to the grid
    for (int i = 1; i <= gridSize * gridSize; i++) {
        // Determine image path from tileTypes array
        const char *path;

        switch (tileTypes[i - 1]) {
        case 0:
            path = tilePaths[tileTypes::lava];
            break;
        case 1:
            path = tilePaths[tileTypes::water];
            break;
        case 2:
            path = tilePaths[tileTypes::lightning];
            break;
        case 3:
            path = tilePaths[tileTypes::ground];
        }

        // Create labels to display each tile via pixmap
        QLabel *label = new QLabel();
        QPixmap image(path);
        label->setPixmap(image);

        // Track the labels so we can destroy them later
        tiles.push_back(label);

        // Add the tile to the grid (filled from left to right)
        ui->glMap->addWidget(label, (i-1) / gridSize, (i-1) % gridSize + 1);
    }

    // Grab the start and finish images
    QPixmap startImage(tilePaths[tileTypes::meReg]);
    QPixmap endImage(tilePaths[tileTypes::finish]);

    // Add the start label to the bottom left
    QLabel *start = new QLabel();
    start->setPixmap(startImage);
    ui->glMap->addWidget(start, gridSize - 1, 0);

    // Add the finish label to the top right
    QLabel *end = new QLabel();
    end->setPixmap(endImage);
    ui->glMap->addWidget(end, 0, gridSize + 1);

    // Track these two as well
    tiles.push_back(start);
    tiles.push_back(end);

    // Remove any padding
    ui->glMap->setMargin(0);
    ui->glMap->setSpacing(0);

    // Resize to minimum requirements
    qApp->processEvents();
    resize(0, 0);

    // Initialize player location
    playerLoc[0] = -1;
    playerLoc[1] = 0;

    debug("Map rendered");
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

    // *When generating tile counts we rounded all numbers up so
    // we might have more than numTiles tiles. Truncate if needed.
    if (grid.size() > numTiles) {
        std::vector<int>::const_iterator first = grid.begin();
        std::vector<int>::const_iterator last = grid.begin() + numTiles;
        std::vector<int> finalGrid(first, last);

        grid = finalGrid;
    }

    return grid;
}

// Generates a string representative of the passed grid
// TODO: The string could be minified by alpha cases + offsets
// Ain't nobody got time for that ^
QString MainWindow::getGridString(std::vector<int> grid) {
    // Lazy alpha representation
    QString gridString;

    for (int i = 0; i < grid.size(); i++) {
        gridString.append(QString::number(grid[i]));
    }

    return gridString;
}

// Outputs debug messages to UI console as well as qDebug
void MainWindow::debug(QString msg) {
    qDebug() << msg;

//    QString curText = ui->txtConsole->toPlainText();
//    ui->txtConsole->setText(msg);
    ui->txtConsole->append(msg);
}

// Clears the UI console
void MainWindow::on_btnClearConsole_clicked()
{
    ui->txtConsole->clear();
}
