/*
    MineCurses - An ncurses minesweeper implementation
    Copyright (C) 2014 ljfa-ag

    This file is part of MineCurses.

    MineCurses is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    MineCurses is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MineCurses.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MCAPP_H_INCLUDED
#define MCAPP_H_INCLUDED

#include "mc_conf.h"
#include "minesweeper.h"

#include <cursesapp.h>

class MineCursesApp : public NCursesApplication
{
public:
    MineCursesApp():
        NCursesApplication(true),
        rt(nullptr),
        msw(nullptr),
        ms(nullptr)
    {}

    ~MineCursesApp();

    int run();
    void handleArgs(int argc, char** argv);

private:
    NCursesWindow* rt;
    NCursesWindow* msw;
    Minesweeper* ms;

    int rows, cols;
    unsigned int mines;

    int ci, cj;

    void init_colors();

    bool init_form();
    void endscreen();

    void movec();
    void draw_cell(int i, int j);
    void draw_cell();
    void draw_endsc_cell(int i, int j);
    void draw_field();

#ifdef MC_WITH_MOUSE
    bool get_mouse(const MEVENT& mevt);
#endif

    void toggle_flag();
    bool make_move();

    Minesweeper::CellEntry& cur_cell();
};

#endif
