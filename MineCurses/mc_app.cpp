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

#include "mc_app.h"
#include "mc_init_form.h"

#include <ctime>
#include <exception>
#include <random>

int MineCursesApp::run()
{
    rt = Root_Window;
    init_colors();
#ifdef MC_WITH_MOUSE
    ::mousemask(ALL_MOUSE_EVENTS, nullptr);
    MEVENT mevt;
#endif

    try
    {
        if(!init_form())
            return 0;

        ms = new Minesweeper(rows, cols);
        msw = new NCursesWindow(rows+2, cols+2, std::max(0, (20-rows-2)/2), (80-cols-2)/2);
        msw->bkgd(' ' | COLOR_PAIR(15));
        msw->box();

        draw_field();
        ci = cj = 0;
        movec();

        bool running = true;
        do
        {
            int ch = msw->getch();
            switch(ch)
            {
            case KEY_LEFT:
                if(--cj < 0)
                    cj = 0;
                movec();
                break;
            case KEY_RIGHT:
                if(++cj >= cols)
                    cj = cols-1;
                movec();
                break;
            case KEY_UP:
                if(--ci < 0)
                    ci = 0;
                movec();
                break;
            case KEY_DOWN:
                if(++ci >= rows)
                    ci = rows-1;
                movec();
                break;

            case '\n':
                running = make_move();
                break;

            case ' ':
                toggle_flag();
                break;

#ifdef MC_WITH_MOUSE
            case KEY_MOUSE:
                if(::getmouse(&mevt) != OK)
                    break;
                if(mevt.bstate & BUTTON1_CLICKED)
                {
                    if(get_mouse(mevt))
                        running = make_move();
                }
                else if(mevt.bstate & BUTTON2_CLICKED)
                {
                    if(get_mouse(mevt))
                        toggle_flag();
                }
                break;
#endif

            case 'q':
            case 27: //escape
                return 0;
            }
        } while(running);

        endscreen();
    }
    catch(std::exception& e)
    {
        rt->clear();
        rt->printw(0, 0, "Error: %s\nPress enter", e.what());
        rt->refresh();
        while(rt->getch() != '\n') {}
        return 1;
    }

    return 0;
}

void MineCursesApp::handleArgs(int argc, char** argv)
{
    //TODO: Allow the game parameters to be passed as command line arguments
}

void MineCursesApp::toggle_flag()
{
    if(!ms->cell(ci, cj).visible())
    {
        ms->cell(ci, cj).flag = !ms->cell(ci, cj).flag;
        draw_cell();
        movec();
        msw->refresh();
    }
}

bool MineCursesApp::make_move()
{
    if(ms->state() == Minesweeper::GameState::uninitialized)
    {
        std::mt19937 rng(std::time(nullptr));
        ms->rand_init(mines, rng, ci, cj);
    }
    if(ms->click(ci, cj))
    {
        draw_field();
        movec();
        msw->refresh();
    }
    return ms->running();
}

void MineCursesApp::init_colors()
{
    init_pair(10, COLOR_WHITE, COLOR_BLACK);    //covered
    init_pair(11, COLOR_WHITE, COLOR_RED);      //flagged
    init_pair(12, COLOR_BLUE, COLOR_WHITE);     //1
    init_pair(13, COLOR_GREEN, COLOR_WHITE);    //2
    init_pair(14, COLOR_RED, COLOR_WHITE);      //3
    init_pair(15, COLOR_BLACK, COLOR_WHITE);    //4
    init_pair(16, COLOR_BLACK, COLOR_WHITE);    //5
    init_pair(17, COLOR_BLACK, COLOR_WHITE);    //6
    init_pair(18, COLOR_BLACK, COLOR_WHITE);    //7
    init_pair(19, COLOR_BLACK, COLOR_WHITE);    //8
    init_pair(20, COLOR_WHITE, COLOR_BLUE);     //end game window
}

bool MineCursesApp::init_form()
{
    MCInitForm infrm;
    infrm();
    if(!infrm.submitted)
        return false;

    rows = infrm.getRowVal();
    cols = infrm.getColVal();
    mines = infrm.getMineVal();
    return true;
}

void MineCursesApp::endscreen()
{
    NCursesWindow endsc(4, 16, 20, 32);
    endsc.bkgd(' ' | COLOR_PAIR(20));
    endsc.box();

    switch(ms->state())
    {
    case Minesweeper::GameState::win:
        endsc.printw(1, 2, "Victory!");
        break;

    case Minesweeper::GameState::loss:
        endsc.printw(1, 2, "Defeat!");
        break;

    default:
        break;
    }

    for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j)
        draw_endsc_cell(i, j);
    msw->refresh();

    endsc.printw(2, 2, "Press enter");
    endsc.refresh();
    while(endsc.getch() != '\n') {}
}

void MineCursesApp::movec()
{
    msw->move(ci+1, cj+1);
}

#ifdef MC_WITH_MOUSE
bool MineCursesApp::get_mouse(const MEVENT& mevt)
{
    int mi = mevt.y - msw->begy() - 1;
    int mj = mevt.x - msw->begx() - 1;
    if(!ms->in_range(mi, mj))
        return false;
    ci = mi;
    cj = mj;
    movec();
    return true;
}
#endif

void MineCursesApp::draw_cell(int i, int j)
{
    int ch;
    if(!ms->cell(i, j).visible())
    {
        if(ms->cell(i, j).flag)
            ch = ' ' | COLOR_PAIR(11);
        else
            ch = ' ' | COLOR_PAIR(10);
    }
    else
    {
        if(ms->cell(i, j).adjacents() == 0)
            ch = ' ' | COLOR_PAIR(12);
        else
            ch = ('0' + ms->cell(i, j).adjacents()) | COLOR_PAIR(11 + ms->cell(i, j).adjacents());
    }
    msw->addch(i+1, j+1, ch);
}

void MineCursesApp::draw_cell()
{
    draw_cell(ci, cj);
}

void MineCursesApp::draw_endsc_cell(int i, int j)
{
    int ch;
    if(ms->cell(i, j).p_mine())
        ch = ' ' | COLOR_PAIR(11);
    else if(ms->cell(i, j).p_adjacents() == 0)
        ch = ' ' | COLOR_PAIR(12);
    else
        ch = ('0' + ms->cell(i, j).p_adjacents()) | COLOR_PAIR(11 + ms->cell(i, j).p_adjacents());
    msw->addch(i+1, j+1, ch);
}

void MineCursesApp::draw_field()
{
    for(int i = 0; i < rows; ++i)
    for(int j = 0; j < cols; ++j)
        draw_cell(i, j);
}

MineCursesApp::~MineCursesApp()
{
    delete msw;
    delete ms;
}

static MineCursesApp mcapp;
