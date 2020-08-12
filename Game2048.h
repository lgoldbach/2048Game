// Copyright 2020, University of Freiburg,
// Author: Leander Goldbach

#ifndef GAME2048_H_
#define GAME2048_H_

#include <getopt.h>
#include <gtest/gtest.h>
#include <vector>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <random>

// Class for visual representation of the game.
class Board {
 public:
    // A default constructor.
    Board();

    // Destructor.
    ~Board();

    void initialize();

    // Draw the input state on the board.
    void drawBoard(std::vector<int> state);

    void drawStats(int steps, int points, bool gameOver, bool gameWon);

 private:
    friend class Game;
    std::vector<std::pair<int, int>> tileCoords;

    // Helper function for update_board: "Animate" the movement of the tiles to
    // interpolate the old and the new state in an aesthetically pleasing way.
    void moveTiles();

    void drawTile(std::pair<int, int> coords);

    // center coordinates of terminal
    int cx;
    int cy;

    float y_scale;  // vertically squish the board by a factor.

    int tileSize;

    int exp(int base, int expo);

    // predefine color-palette (131072 is max possible tile value in 4x4
    std::unordered_map<int, int> const colors
        {{ 0, 231 }, { 2, 230 },
        { 4, 228 }, { 8, 220 },
        { 16, 215 }, { 32, 202 },
        { 64, 169 }, { 128, 221 },
        { 256, 214 }, { 512, 178 },
        { 1024, 172 }, { 2048, 136 },
        { 4096, 236 }, { 8192, 236 },
        { 16384, 236 }, { 32768, 236 },
        { 65536, 236 }, { 131072, 236 }
    };
};


// Class handling the internal logic of the game
class Game {
 public:
    // initialize the Game with a random board setup (Calls Board constructor).
    Game();

    // Destructor;
    ~Game();

    // Count steps.
    int steps;

    // Count points.
    std::vector<int> points;

    bool gameOver;

    bool gameWon;

    // core function that takes user input and advances the game.
    bool play(int key);

    // Keep a reference to all the states that existed during the game.
    // to inner vectors each represent one state. Whenever the player
    // makes a move, we append the new state as a vector to the end.
    std::vector<std::vector<int>> states;

    void parseCommandLineArguments(int argc, char** argv);

    int maxUndo;

    bool run;

 private:
    FRIEND_TEST(Game2048Test, constructor);
    FRIEND_TEST(Game2048Test, play);
    FRIEND_TEST(Game2048Test, updateState);
    FRIEND_TEST(Game2048Test, addRandom);
    FRIEND_TEST(Game2048Test, undo);
    FRIEND_TEST(Game2048Test, reset);
    FRIEND_TEST(Game2048Test, isGameOver);
    FRIEND_TEST(Game2048Test, gameWon);

    int undoCount;

    // reference to the board.
    Board board;

    // the hashlists needed for each move (up, down, right, left).
    std::unordered_map<int, std::vector<int>> const hashLists {
            { 1, { 0, 1, 2, 3 } }, { 2, { 4, 5, 6, 7 } },
            { 3, { 8, 9, 10, 11 } }, { 4, { 12, 13, 14, 15 } },
            { 5, { 3, 2, 1, 0 } }, { 6, { 7, 6, 5, 4 } },
            { 7, { 11, 10, 9, 8 } }, { 8, { 15, 14, 13, 12 } },
            { 9, { 0, 4, 8, 12 } }, { 10, { 1, 5, 9, 13 } },
            { 11, { 2, 6, 10, 14 } }, { 12, { 3, 7, 11, 15 } },
            { 13, { 12, 8, 4, 0 } }, { 14, { 13, 9, 5, 1 } },
            { 15, { 14, 10, 6, 2 } }, { 16, { 15, 11, 7, 3 } },
    };

    // function for each possible move.
    void updateState(std::vector<int> &newState, std::vector<int> &oldState,
            const int move);

    void updateSlice(std::vector<int> &newState, std::vector<int> &oldState,
            std::vector<int> hash);

    void insert(std::vector<int> &state, int val, std::vector<int> hash);

    void addRandom(std::vector<int> &state);

    const int randomField();

    const int randomVal();

    const void printUsageAndExit();

    void undo();

    void reset();

    bool isGameOver();

    const std::vector<int> coreTiles { 5, 6, 9, 10 };
};

#endif  // GAME2048_H_
