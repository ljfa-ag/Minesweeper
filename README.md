This repository consists of libminesweeper and MineCurses.

# libminesweeper
A free library which implements a minesweeper framework. It manages the game logic
and provides an interface for making game moves.

# MineCurses
A simple minesweeper game using libminesweeper and ncurses. You can specify the field
size and the number of mines at the beginning of the game.

It also supports mouse input by defining MC_WITH_MOUSE, though that may be bugged
on some implementations of ncurses.
