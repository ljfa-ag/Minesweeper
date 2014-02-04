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

#ifndef MCINITFORM_H_INCLUDED
#define MCINITFORM_H_INCLUDED

#include "mc_conf.h"

#include <cursesf.h>

#include <cstdlib>
#include <string>

class Label : public NCursesFormField
{
public:
    Label(const std::string& title, int first_row, int first_col):
        NCursesFormField(1, title.size(), first_row, first_col)
    {
        set_value(title.c_str());
        options_off(O_EDIT | O_ACTIVE);
    }
};

class MCInitForm : public NCursesForm
{
public:
    bool submitted;

    MCInitForm();
    ~MCInitForm() {}

    int virtualize(int c);

    int getRowVal() { return std::atoi(mFields[3]->value()); }
    int getColVal() { return std::atoi(mFields[4]->value()); }
    int getMineVal() { return std::atoi(mFields[5]->value()); }

private:
    NCursesFormField** mFields;
    Integer_Field mRowF, mColF, mMineF;
};

#endif
