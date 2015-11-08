#include <QtDebug>
#include <QKeyEvent>

#include <algorithm>
#include <cmath>

#include "mainwindow.h"

enum state {
    regular,
    electric,
    fire,
    wet
};

enum tileState {
    lavaTile,
    waterTile,
    electricTile,
    groundTile
};

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
    if (finished) { return; }

    int key = event->key();

    if (key == Qt::Key_Left || key == Qt::Key_A) {
        move(0);
    }
    else if (key == Qt::Key_Right || key == Qt::Key_D) {
        move(1);
    }
    else if (key == Qt::Key_Up || key == Qt::Key_W) {
        move(2);
    }
    else if (key == Qt::Key_Down || key == Qt::Key_S) {
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
            return false;
        }
    }

    int newLoc[2] = {playerLoc[0], playerLoc[1]};

    switch (direction) {
    // Left
    case 0:
        newLoc[0]--;
        break;
    // Right
    case 1:
        newLoc[0]++;
        break;
    // Up
    case 2:
        newLoc[1]--;
        break;
    // Down
    case 3:
        newLoc[1]++;
        break;
    }

    // We've gone off the map
    if (newLoc[0] < 0 || newLoc[1] < 0 || newLoc[1] >= mapSize) {
        debug("Bump");
        return false;
    }

    if (newLoc[0] == mapSize && newLoc[1] < mapSize - 1) {
        debug("Bump");
        return false;
    }

    // Normal movement
    updatePosition(playerLoc, newLoc);

    playerLoc[0] = newLoc[0];
    playerLoc[1] = newLoc[1];

    // We've gone off the map... AND REACHED THE FINISH!
    if (newLoc[0] == mapSize) {
        // Finish routine
        return true;
    }
}

// Graphically update the player position
void MainWindow::updatePosition(int oldLoc[2], int newLoc[2]) {
    int oldLocFlat = oldLoc[0] + oldLoc[1] * mapSize;
    int newLocFlat = newLoc[0] + newLoc[1] * mapSize;

    // Simply clear old loc if we're moving off the start
    if (oldLoc[0] == -1) {
        tiles[mapSize * mapSize]->clear();
    }
    // Otherwise, replace it from whence it came
    else {
        const char *path;

        switch (tileIDs[oldLocFlat]) {
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
            break;
        }

        QPixmap image(path);
        tiles[oldLocFlat]->setPixmap(image);
    }

    // If the new location is the finish, no update is needed for it
    if (newLoc[0] == mapSize) {
        debug("Finished!");
        finished = true;
        return;
    }

    // We're now off the previous tile and haven't stepped on the next one - let's check state
    int curState = playerState;
    int newTileState = tileIDs[newLocFlat];

    // State/Point logic
    switch (newTileState) {
    
    case tileState::lavaTile:
        // If the player is wet, this dries them off
        if (playerState == state::wet) {
            playerState = state::regular;
            stateTimer = -1; // Deactivate the water timer
            
            debug("You dried off!");
        }
        // Otherwise it sets them on fire
        else {
            playerState = state::fire;
            stateTimer = 5; // State timer is reset to 5
            
            debug("You caught fire!");
        }
        
        break;

    case tileState::electricTile:
        // If the player is wet, they're electrocuted
        if (playerState == state::wet) {
            // State stays the same but score get knickered
            score += 5;
            
            debug("BZZZZT (+5)");
        }
        // If the player is regular, they build some charge
        else if (playerState == state::regular) {
            playerState = state::electric;
            stateTimer = 2;
            
            debug("You built up some static charge.");
        }
        // If the player is already charged, they build some more
        else if (playerState == state::electric) {
            stateTimer += 2;
            
            if (stateTimer >= 10) {
                debug("YOU ARE A GOD AMONG MEN! (avoid water)");
            }
            else {
                debug("Your charge grows stronger.");
            }
        }
        // Otherwise, nothing interesting happens
        else {
            debug("Your flaming feet tingle a bit...");
        }
        break;
        
    case tileState::waterTile:
        // If the player is charged, they're electrocuted
        if (playerState == state::electric) {
            // State switches to wet and they're bzappp'd
            playerState = state::wet;
            int damage = stateTimer + 1;
            score += damage;
            stateTimer = 5;

            debug("BZZZGGRRRBBGPGP! (+" + QString::number(damage) + ")");
        }
        // If the player is on fire, they put it out
        else if (playerState == state::fire) {
            playerState = state::regular;
            stateTimer = -1;

            debug("You put out the fire!");
        }
        // Otherwise, be wet
        else {
            playerState = state::wet;
            stateTimer = 5;

            debug("You're drenched!");
        }

        break;
        
    case tileState::groundTile:
        // If they're charged, they get grounded
        if (playerState == state::electric) {
            playerState = state::regular;
            stateTimer = -1;

            debug("You were grounded.");
        }

        break;
    }

    // Fire hurts
    if (stateTimer >= 0 && playerState == state::fire) {
        score += 2;

        debug("The fire burns! (+2)");
    }

    // Decrement state timer
    if (stateTimer >= 0) { stateTimer--; }

    // Deactivate state if needed
    if (stateTimer == -1 && playerState != state::regular) {
        playerState = state::regular;
    }

    // Increment score and update labels
    score++;

    ui->lblScore->setText("Score: " + QString::number(score));

    QString stateText = "State: Regular";

    if (playerState == state::fire) {
        stateText = "State: Fire (" + QString::number(stateTimer) + ")";
    }
    else if (playerState == state::wet) {
        stateText = "State: Wet (" + QString::number(stateTimer) + ")";
    }
    else if (playerState == state::electric) {
        stateText = "State: Electric (" + QString::number(stateTimer) + ")";
    }

    ui->lblState->setText(stateText);

    const char *path;

    switch (tileIDs[newLocFlat]) {
    case tileState::lavaTile:
        path = getPlayerTile(tileTypes::lava, playerState);
        break;
    case tileState::waterTile:
        path = getPlayerTile(tileTypes::water, playerState);
        break;
    case tileState::electricTile:
        path = getPlayerTile(tileTypes::lightning, playerState);
        break;
    case tileState::groundTile:
        path = getPlayerTile(tileTypes::ground, playerState);
        break;
    }

    QPixmap image(path);
    tiles[newLocFlat]->setPixmap(image);
}

// Get the proper tile based on the current player state
// This logic could be reduced by drawing the player directly
// on to the tile, but we're not here to draw now, are we?
const char * MainWindow::getPlayerTile(int tileType, int curState) {
    switch (tileType) {
    case tileTypes::lava:

        switch (curState) {
        case state::regular:
            return tilePaths[tileTypes::lavaReg];
            break;
        case state::electric:
            return tilePaths[tileTypes::lavaElec];
            break;
        case state::fire:
            return tilePaths[tileTypes::lavaFire];
            break;
        case state::wet:
            return tilePaths[tileTypes::lavaWet];
            break;
        }

        break;
    case tileTypes::water:

        switch (curState) {
        case state::regular:
            return tilePaths[tileTypes::waterReg];
            break;
        case state::electric:
            return tilePaths[tileTypes::waterElec];
            break;
        case state::fire:
            return tilePaths[tileTypes::waterFire];
            break;
        case state::wet:
            return tilePaths[tileTypes::waterWet];
            break;
        }

        break;
    case tileTypes::lightning:

        switch (curState) {
        case state::regular:
            return tilePaths[tileTypes::lightningReg];
            break;
        case state::electric:
            return tilePaths[tileTypes::lightningElec];
            break;
        case state::fire:
            return tilePaths[tileTypes::lightningFire];
            break;
        case state::wet:
            return tilePaths[tileTypes::lightningWet];
            break;
        }

        break;
    case tileTypes::ground:

        switch (curState) {
        case state::regular:
            return tilePaths[tileTypes::groundReg];
            break;
        case state::electric:
            return tilePaths[tileTypes::groundElec];
            break;
        case state::fire:
            return tilePaths[tileTypes::groundFire];
            break;
        case state::wet:
            return tilePaths[tileTypes::groundWet];
            break;
        }

        break;
    }

    // Should never happen (and will be obvious if it does)
    return tilePaths[tileTypes::finish];
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

    // Map size/type
    mapSize = -1;
    mapType = ui->cboMapType->currentIndex();

    // Determine size based on user selection
    switch (ui->cboMapSize->currentIndex()) {
    case 0:
        mapSize = 10;
        break;
    case 1:
        mapSize = 12;
        break;
    case 2:
        mapSize = 15;
        break;
    }

    // Should never happen
    if (mapSize == -1) {
        qWarning("Unable to detect size, defaulting to small");
        mapSize = 10;
    }

    // Generate grid
    tileIDs = generateGrid(mapType, mapSize);

    debug(QString::number(tileIDs.size()) + " tiles generated");

    // Get representative string for generated grid
    ui->txtMapCode->setText(getGridString(tileIDs));

    debug("Map import string generated");

    // Create tile images and add them to the grid
    for (int i = 1; i <= mapSize * mapSize; i++) {
        // Determine image path from tileTypes array
        const char *path;

        switch (tileIDs[i - 1]) {
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
        ui->glMap->addWidget(label, (i-1) / mapSize, (i-1) % mapSize + 1);
    }

    // Grab the start and finish images
    QPixmap startImage(tilePaths[tileTypes::meReg]);
    QPixmap endImage(tilePaths[tileTypes::finish]);

    // Add the start label to the bottom left
    QLabel *start = new QLabel();
    start->setPixmap(startImage);
    ui->glMap->addWidget(start, 0, 0);

    // Add the finish label to the top right
    QLabel *end = new QLabel();
    end->setPixmap(endImage);
    ui->glMap->addWidget(end, mapSize - 1, mapSize + 1);

    // Track these two as well
    tiles.push_back(start);
    tiles.push_back(end);

    // Remove any padding
    ui->glMap->setMargin(0);
    ui->glMap->setSpacing(0);

    // Resize to minimum requirements
    qApp->processEvents();
    resize(0, 0);

    // Initialize player location/state
    playerLoc[0] = -1;
    playerLoc[1] = 0;
    playerState = state::regular;
    stateTimer = -1;
    score = 0;
    finished = false;

    debug("Map rendered");
}

// Randomly generates the game grid
// type argument can take five values
// 0: Normal - even distribution of tile types
// 1: Volcano - 50% lava tiles
// 2: Oasis - 50% water tiles
// 3: Factory - 50% electric tiles
// 4: Forest - 50% ground tiles
std::vector<int> MainWindow::generateGrid(int type, int mapSize) {
    // Determine minimum number of tiles for any map type
    int numTiles = mapSize * mapSize;
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
