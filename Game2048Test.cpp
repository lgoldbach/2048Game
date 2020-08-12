// Copyright 2020, University of Freiburg,
// Author: Leander Goldbach

#include <gtest/gtest.h>
#include <vector>
#include <iostream>
#include "./Game2048.h"


// ____________________________________________________________________________
TEST(Game2048Test, constructor) {
    {
        Game game = Game();
        int count = 0;
        for (int i = 0; i < game.states[0].size(); i++) {
            if (game.states[0][i] == 2 || game.states[0][i] == 4) {
                count += 1;
            }
        }
        // check if we really got exactly two fields with 2 or 4.
        ASSERT_EQ(2, count);
    }
}

// ____________________________________________________________________________
TEST(Game2048Test, play) {
    Game game = Game();
    game.play(10);  // do nothing
    game.play(10);
    ASSERT_EQ(1, game.states.size());

    game.states[0][15] = 2;  // set 2 at bottom.
    game.play(259);  // arrow up
    ASSERT_EQ(2, game.states.size());
}

// ____________________________________________________________________________
TEST(Game2048Test, updateState) {
    Game game = Game();

    // column 1
    game.states[0][0] = 2;
    game.states[0][4] = 2;
    game.states[0][8] = 4;
    game.states[0][12] = 4;
    // column 2
    game.states[0][1] = 2;
    game.states[0][5] = 0;
    game.states[0][9] = 0;
    game.states[0][13] = 2;
    // column 3
    game.states[0][2] = 0;
    game.states[0][6] = 0;
    game.states[0][10] = 0;
    game.states[0][14] = 2;
    // column 4
    game.states[0][3] = 2;
    game.states[0][7] = 4;
    game.states[0][11] = 0;
    game.states[0][15] = 4;

    game.play(259);  // arrow up
    // column 1
    ASSERT_EQ(4, game.states[1][0]);
    ASSERT_EQ(8, game.states[1][4]);
    // column 2
    ASSERT_EQ(4, game.states[1][1]);
    // column 3
    ASSERT_EQ(2, game.states[1][2]);
    // column 4
    ASSERT_EQ(2, game.states[1][3]);
    ASSERT_EQ(8, game.states[1][7]);
}

// ____________________________________________________________________________
TEST(Game2048Test, addRandom) {
    Game game = Game();
    game.addRandom(game.states[0]);
    int count = 0;
    for (int i = 0; i < 16; i++) {
        if (game.states[0][i] > 0) count++;
    }
    ASSERT_EQ(3, count);
}


// ____________________________________________________________________________
TEST(Game2048Test, undo) {
    Game game = Game();
    game.maxUndo = 1;

    int steps1 = game.steps;
    int points1 = game.points.back();

    game.states[0][15] = 2;  // set 2 at bottom (force a change when KEY_UP).

    std::vector<int> vals1;
    for (int i = 0; i < 16; i++) {
        if (game.states[0][i] > 0) vals1.push_back(game.states[0][i]);
    }

    game.play(259);  // KEY_UP

    int steps2 = game.steps;
    int points2 = game.points.back();

    std::vector<int> vals2;
    for (int i = 0; i < 16; i++) {
        if (game.states[0][i] > 0) vals2.push_back(game.states[0][i]);
    }
    game.undo();
    int steps3 = game.steps;
    int points3 = game.points.back();

    std::vector<int> vals3;
    for (int i = 0; i < 16; i++) {
        if (game.states[0][i] > 0) vals3.push_back(game.states[0][i]);
    }

    ASSERT_EQ(steps1, steps3);
    ASSERT_EQ(points1, points3);
    ASSERT_NE(steps2, steps3);

    ASSERT_EQ(vals1[0], vals3[0]);
    ASSERT_EQ(vals1[1], vals3[1]);
}


// ____________________________________________________________________________
TEST(Game2048Test, reset) {
    Game game = Game();
    game.play(259);
    game.points.push_back(4);  // simulate scoring points

    game.reset();
    ASSERT_EQ(0, game.steps);
    ASSERT_EQ(0, game.points.back());
}

TEST(Game2048Test, isGameOver) {
    Game game = Game();
    for (int i = 1; i < 17; i++) {
        game.states[0][i-1] = i;
    }
    ASSERT_EQ(true, game.isGameOver());
}

// ____________________________________________________________________________
TEST(Game2048Test, gameWon) {
    Game game = Game();
    ASSERT_EQ(false, game.gameWon);

    game.states[0][4] = 2048;
    game.states[0][0] = 0;
    game.play(259);

    ASSERT_EQ(true, game.gameWon);

    // we won but we can continue to play
    game.states[1][4] = 2048;
    game.play(259);
    ASSERT_EQ(4096, game.states[2][0]);
}
