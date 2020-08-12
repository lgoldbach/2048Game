# 2048Game
The classic 2048 game. My final programming project for 'Advanced Programming (C++)' course at University of Freiburg (Summer 2020).

Run the Game
============
Clone the project including the Makefile, cd into the directory, enter "make" into command line and then enter "./Game2048Main" .
"./Game2048Main -u [n]" gives you n consecutive undos while playing (e.g. "./Game2048 -u 3" gives you 3 undos, default is 0 undos).
"./Game2048Main -h" for usage.

Required:\
ncurses\
gtest  (comment all 'test' out in the Makefile to ignore tests)
  
Play the Game
=============
Use arrow keys to play, press N to start a new game, press ESC to exit game, press U to undo a move (only works when -u [n] flag was used with ./Game2048Main)
