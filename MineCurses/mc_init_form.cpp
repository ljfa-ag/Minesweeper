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

#include "mc_init_form.h"

MCInitForm::MCInitForm():
    NCursesForm(8, 27, 8, 26),
    submitted(false),
    mFields(new NCursesFormField*[8]),
    mRowF(0, 1, 22),
    mColF(0, 1, 78),
    mMineF(0, 1, 999)
{
    mFields[0] = new Label("Field height (1-22):", 0, 0);
    mFields[1] = new Label("Field witdth (1-78):", 1, 0);
    mFields[2] = new Label("Number of mines:", 3, 0);

    mFields[3] = new NCursesFormField(1, 3, 0, 21);
    mFields[4] = new NCursesFormField(1, 3, 1, 21);
    mFields[5] = new NCursesFormField(1, 4, 3, 17);

    mFields[6] = new NCursesFormField(1, 5, 5, 10);

    mFields[7] = new NCursesFormField;

    InitForm(mFields, true, true);
    boldframe();
    label("New game", "");

    mFields[3]->set_fieldtype(mRowF);
    mFields[4]->set_fieldtype(mColF);
    mFields[5]->set_fieldtype(mMineF);

    mFields[6]->set_value("Start");
    mFields[6]->options_off(O_EDIT);
}

int MCInitForm::virtualize(int c)
{
    switch(c)
    {
    case 8: case KEY_BACKSPACE: return REQ_DEL_PREV; //backspace
    case '\t':      return REQ_NEXT_FIELD;

    case KEY_LEFT:  return REQ_LEFT_CHAR;
    case KEY_RIGHT: return REQ_RIGHT_CHAR;
    case KEY_UP:    return REQ_UP_FIELD;
    case KEY_DOWN:  return REQ_DOWN_FIELD;

    case KEY_HOME:  return REQ_BEG_FIELD;
    case KEY_END:   return REQ_END_FIELD;

    case KEY_DC:    return REQ_DEL_CHAR;
    case 'q': case 27: return MAX_COMMAND + 1; //escape

    case '\n':
        if(current_field() == mFields[6])
        {
            submitted = true;
            return MAX_COMMAND + 1;
        }
        else
            return REQ_NEW_LINE;

    default:
        return c;
    }
}
