# 2048Game
The classic 2048 game. My final programming project for Advanced Programming (C++) course at University of Freiburg (Summer 2020).

Run the Game
============
Clone the project including the Makefile, cd into dir., enter "make" command line and then enter ./Game2048Main .
./Game2048 -u [n] give you 3 consecutive undos while playing.
./Game2048 -h for usage.

Required:
  ncurses
  gtest  (comment all 'test' out in the Makefile to ignore tests)
  
Play the Game
=============
Use arrow keys to play, press N to start a new game, press ESC to exit game, press U to undo a move (only works when -u [n] flag was used with ./Game2048Main)
