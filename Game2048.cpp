// Copyright 2020, University of Freiburg,
// Author: Leander Goldbach

#include <getopt.h>
#include <ncurses.h>
#include <vector>
#include <utility>
#include <iostream>
#include <unordered_map>
#include <random>
#include <string>
#include "./Game2048.h"

Board::Board() {
    // coordinates for tile centers relative to terminal center.
    std::vector<int> c = { -36, -12, 12, 36 };
    tileSize = c.back()/4;

    y_scale = 2.3;  // vertically squish the board by a factor.
    // Save coordinate pairs in a vector.
    for (const auto& i : c) {
        for (const auto& j : c) {
            std::pair<int, int> p(i/y_scale, j);
            tileCoords.push_back(p);
        }
    }
}

void Board::initialize() {
    initscr();              // Initializiation.
    start_color();          // init colors.
    use_default_colors();   // default terminal colors.
    cbreak();               // Don't wait for RETURN.
    noecho();               // Don't echo key presses on screen.
    curs_set(false);        // Don't show the cursor.
    nodelay(stdscr, true);  // Don't wait until key pressed.
    keypad(stdscr, true);   // For KEY_LEFT, KEY_UP, etc.

    // get center coordinates.
    cx = COLS / 2;
    cy = LINES / 2;

    init_pair(100, COLOR_BLACK, COLOR_WHITE);
}

void Board::drawBoard(std::vector<int> state) {
    for (int i = 0; i < 16; i ++) {
        init_pair(i+1, colors.at(state[i]), colors.at(state[i]));
        init_pair(i+1+16, 232, colors.at(state[i]));

        attron(COLOR_PAIR(i+1));
        drawTile(tileCoords[i]);
        attroff(COLOR_PAIR(i+1));

        if (state[i] != 0) {
            attron(COLOR_PAIR(i + 1 + 16));
            mvprintw(cy + tileCoords[i].first, cx + tileCoords[i].second, "%d",
                     state[i]);
            attroff(COLOR_PAIR(i + 1 + 16));
        }
    }
}

void Board::drawTile(std::pair<int, int> coords) {
    for (int i = -tileSize; i <= tileSize; i++) {
        for (int j = -tileSize; j <= tileSize; j++) {
            mvprintw(cy+coords.first+(i/y_scale), cx+coords.second+j, " ");
        }
    }
}

void Board::drawStats(int steps, int points, bool gameOver, bool gameWon) {
    mvprintw(cy-5, 3, "Round %d     ", steps);
    mvprintw(cy-4, 3, "Points: %d     ", points);
    mvprintw(cy-3, 3, "         ");
    if (gameOver) mvprintw(cy-3, 3, "GAME OVER");
    mvprintw(cy-2, 3, "                                               ");
    if (gameWon) mvprintw(cy-2, 3, "You've reached 2048 and won! Yay!");
}

Board::~Board() {
    // deconstructor.
}

// init. the game by randomly selecting two fields and then a 4 or a 2 for
// each field with probabilities 10% and 90% respectively.
Game::Game() {
    run = true;
    maxUndo = 0;  // default undo limit.
    steps = 0;  // set setp counter to 0.
    gameOver = false;
    gameWon = false;
    points.push_back(0);

    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd());  // seed the generator
    std::uniform_int_distribution<> distr(0, 15);  // define range

    // get to random fields
    int fieldOne = distr(gen);
    int fieldTwo = distr(gen);


    // Build the current state
    std::vector<int> state(16, 0);
    int field1 = randomField();
    state[field1] = randomVal();
    int field2 = randomField();
    while (field2 == field1) field2 = randomField();
    state[field2] = randomVal();

    // Add the first state.
    states.push_back(state);
}

Game::~Game() {
    //
}

// ___________________________________________________________________________
bool Game::play(int key) {
    // init the new state
    std::vector<int> newState(16, 0);
    // get the old state (last item in states)
    std::vector<int> oldState = states.back();

    switch (key) {
        case KEY_LEFT: if (!gameOver) updateState(newState, oldState, 1);
        return true;
        case KEY_RIGHT: if (!gameOver) updateState(newState, oldState, 2);
        return true;
        case KEY_UP: if (!gameOver) updateState(newState, oldState, 3);
        return true;
        case KEY_DOWN: if (!gameOver) updateState(newState, oldState, 4);
        return true;
        case 'u': undo(); return true;
        case 27: run = false; return false;
        case 'n': reset(); return true;
    }
    return false;
}

void Game::updateState(std::vector<int> &newState, std::vector<int> &oldState,
        const int move) {
    // hash
    std::vector<int> hashs;

    // depending on the move we have to consider a different slices of the
    // board in different order. The numbers are used to get the relevant
    // indices from hashLists, which stores each possible slice in a
    // dictionary type data structure.
    if ( move == 1 ) {
        hashs = { 1, 2, 3, 4 };
    }
    if ( move == 2 ) {
        hashs = { 5, 6, 7, 8 };
    }
    if ( move == 3 ) {
        hashs = { 9, 10, 11, 12 };
    }
    if ( move == 4 ) {
        hashs = { 13, 14, 15, 16 };
    }
    // update all the relevant slices.
    for (const auto& idx : hashs) {
        std::vector<int> hash = hashLists.at(idx);
        updateSlice(newState, oldState, hash);
    }

    // only update if state was changed by click.
    if (newState != oldState) {
        steps += 1;
        undoCount = 0;
        addRandom(newState);  // add a new random number (4/2)
        states.push_back(newState);  // add the new state
        gameOver = isGameOver();
    }
}

void Game::updateSlice(std::vector<int> &newState, std::vector<int> &oldState,
        std::vector<int> hash) {
    // loop for first three elements
    for (int i = 0; i < 4; i++) {
        // ignore zeros.
        if (oldState[hash[i]] == 0) continue;
        // if we reached last field, always add it (if >0 obv.)
        if (i == 3) {
            insert(newState, oldState[hash[i]], hash);
            break;
        }
        // loop over i+1 element to the fourth/last element.
        for (int j = i+1; j < 4; j++) {
            // if two numbers match, (i.e. when i=0, j=1 and oldState=2200)
            // or when i=0, j=2 and oldState = 4040)
            if (oldState[hash[i]] == oldState[hash[j]]) {
                insert(newState, oldState[hash[i]] + oldState[hash[j]], hash);
                points.push_back(points.back() +
                oldState[hash[i]] + oldState[hash[j]]);
                i = j;  // set i to j so at next loop i will be at j+1.
                break;  // break the j-loop and continue i-loop at j+1.
                // if ith and jth element don't match
            } else {
                // if jth element is not-zero, we have to add ith element.
                if (oldState[hash[j]] > 0) {
                    insert(newState, oldState[hash[i]], hash);
                    break;
                } else {
                    // add ith ele. if jth element is zero but end is reached.
                    if (j == 3) {
                        insert(newState, oldState[hash[i]], hash);
                    }
                }
            }
        }
    }
}

void Game::insert(std::vector<int> &state, int val, std::vector<int> hash) {
    // insert value at lowest index possible.
    for (int p = 0; p < 4; p++) {
        if (state[hash[p]] == 0) {
            state[hash[p]] = val;
            break;
        }
    }
}

// check if no more moves are possible.
bool Game::isGameOver() {
    // first check if the board is even full.
    bool boardFull = true;
    for (int i = 0; i < 16; i++) {
        // only set gameWon to true at first 2048.
        if (states.back()[i] == 2048 && !gameWon) gameWon = true;
        if (states.back()[i] == 0) {
            boardFull = false;
        }
    }
    if (boardFull) {
        // check if any two neighboring (not diag.) tiles have same val.
        bool stuck = true;
        for (const auto& ct : coreTiles) {
            if (states.back()[ct] == states.back()[ct-4] ||
                states.back()[ct] == states.back()[ct+4] ||
                states.back()[ct] == states.back()[ct+1] ||
                states.back()[ct] == states.back()[ct-1]) {
                stuck = false;
            }
        }
        if (states.back()[0] == states.back()[1] ||
            states.back()[0] == states.back()[4] ||
            states.back()[3] == states.back()[2] ||
            states.back()[3] == states.back()[7] ||
            states.back()[12] == states.back()[13] ||
            states.back()[12] == states.back()[8] ||
            states.back()[15] == states.back()[14] ||
            states.back()[15] == states.back()[11]) {
            stuck = false;
        }
        return stuck;
    } else {
        return false;
    }
}

void Game::addRandom(std::vector<int> &state) {
    int field;
    while (true) {
        std::uniform_int_distribution<> distr(0, 15);  // define range
        // get to random fields
        field = randomField();

        // if found empty field we're done.
        if (state[field] == 0) break;
    }
    state[field] = randomVal();
}

void Game::undo() {
    if (undoCount >= maxUndo) return;
    if (states.size() <= 1) return;
    gameOver = false;
    states.pop_back();
    if (points.size() > 1) points.pop_back();
    steps--;
    undoCount++;
}

// _______________________________________________________________________
void Game::parseCommandLineArguments(int argc, char** argv) {
    struct option options[] = {
            { "max-undo", 1, NULL, 'u'},
            { "help", 1, NULL, 'h'},
            { NULL, 0, NULL, 0 }
    };
    optind = 1;
    while (true) {
        char c = getopt_long(argc, argv, "u:h", options, NULL);
        if (c == -1) break;
        switch (c) {
            case 'u':
                maxUndo = atoi(optarg);
                break;
            case 'h':
                printUsageAndExit();
                break;
        }
    }
}

void Game::reset() {
    run = true;
    steps = 0;  // set setp counter to 0.
    gameOver = false;
    gameWon = false;
    points.clear();
    points.push_back(0);

    // Build the current state
    std::vector<int> state(16, 0);
    int field1 = randomField();
    state[field1] = randomVal();
    int field2 = randomField();
    while (field2 == field1) field2 = randomField();
    state[field2] = randomVal();

    // clear
    states.clear();
    // Add the first state.
    states.push_back(state);
}

const int Game::randomField() {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd());  // seed the generator
    std::uniform_int_distribution<> distr(0, 15);  // define range

    int field;
    field = distr(gen);
    return field;
}

const int Game::randomVal() {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd());  // seed the generator

    std::uniform_int_distribution<> distr(1, 10);  // redefine range

    // random numbers to be put into the two fields.
    int num;

    // 4 has 10% prob, 2 has 90% prob.
    int rand = distr(gen);
    if (rand == 10) {
        num = 4;
    } else {
        num = 2;
    }
    return num;
}

const void Game::printUsageAndExit() {
    std::cout << "Usage: Game2048Main [optional] max-undo [-u]\nDefault = 1.\n";
}
