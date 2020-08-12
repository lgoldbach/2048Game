// Copyright 2020, University of Freiburg,
// Author: Leander Goldbach

#include <ncurses.h>
#include <unistd.h>
#include "./Game2048.h"

int main(int argc, char** argv) {
    std::cout << argv[0] << std::endl;
    Game game = Game();
    game.parseCommandLineArguments(argc, argv);
    Board board = Board();
    board.initialize();
    board.drawBoard(game.states.back());
    board.drawStats(game.steps, game.points.back(),
            game.gameOver, game.gameWon);

    bool change;
    while (game.run) {
        int key = getch();  // get key press event
        change = game.play(key);  // update game based on user input
        if (change) {
            board.drawBoard(game.states.back());
            board.drawStats(game.steps, game.points.back(),
                    game.gameOver, game.gameWon);
        }
    }
    endwin();
}
