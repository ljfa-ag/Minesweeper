/*
    libminesweeper
    Copyright (C) 2014 ljfa-ag

    This file is part of libminesweeper.

    libminesweeper is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libminesweeper is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libminesweeper.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "minesweeper.h"

#include <iomanip>

int Minesweeper::CellEntry::adjacents() const
{
    if(mVisible)
        return mAdjacents;
    else
        return -1;
}

Minesweeper::CellEntry::CellEntry(bool mine, bool visible, int adjacents, bool flag):
    flag(flag),
    mMine(mine),
    mVisible(visible),
    mAdjacents(adjacents)
{}

Minesweeper::Minesweeper(int rows, int cols):
    mRows(rows),
    mCols(cols),
    mData(rows*cols, CellEntry()),
    mState(GameState::uninitialized)
{
    if(rows <= 0 || cols <= 0)
        throw std::out_of_range("The number of rows and columns must be positive");
}

void Minesweeper::init()
{
    unsigned int mines = 0;
    for(int i = 0; i < mRows; ++i)
    for(int j = 0; j < mCols; ++j)
    {
        if(cell(i, j).mMine)
        {
            ++mines;
            continue;
        }
        //Compute the number of mines in the neighbor fields
        int adj = 0;
        for_each_nb_in_range(i, j, [&](int k, int l)
            { adj += try_get_cell(k, l).mMine; });
        cell(i, j).mAdjacents = adj;
    }
    mCovered = cells() - mines;
    mState = GameState::running;
}

bool Minesweeper::in_range(int i, int j) const
{
    return 0 <= i && i < mRows && 0 <= j && j < mCols;
}

auto Minesweeper::cell(int i, int j) -> CellEntry&
{
    return mData[i*mCols + j];
}

auto Minesweeper::cell(int i, int j) const -> const CellEntry&
{
    return mData[i*mCols + j];
}

auto Minesweeper::try_get_cell(int i, int j) const -> CellEntry
{
    if(in_range(i, j))
        return cell(i, j);
    else
        return CellEntry();
}

bool Minesweeper::uncover(int i, int j)
{
    if(cell(i, j).mVisible)
        return false;
    if(cell(i, j).mMine)
    {
        mState = GameState::loss;
        return false;
    }
    if(p_uncover(i, j) && cell(i, j).mAdjacents == 0)
        p_rec_uncover();
    return true;
}

void Minesweeper::p_rec_uncover()
{
    bool cont;
    do
    {
        cont = false;
        for(int i = 0; i < mRows; ++i)
        for(int j = 0; j < mCols; ++j)
        {
            if(!cell(i, j).mVisible || cell(i, j).mAdjacents != 0)
                continue;
            for_each_nb_in_range(i, j, [&](int k, int l){ cont = p_uncover(k, l) || cont; });
        }
    } while(cont);
}

bool Minesweeper::p_uncover(int i, int j)
{
    if(cell(i, j).mVisible)
        return false;
    cell(i, j).mVisible = true;
    if(--mCovered == 0)
        mState = GameState::win;
    return true;
}

bool Minesweeper::uncover_if_unmarked(int i, int j)
{
    if(!cell(i, j).flag)
        return uncover(i, j);
    else
        return false;
}

bool Minesweeper::chord(int i, int j)
{
    if(!cell(i, j).mVisible)
        return false;
    //Compute the number of adjacent flagged cells
    int markeds = 0;
    for_each_nb_in_range(i, j, [this, &markeds](int k, int l) { markeds += cell(k, l).flag; });
    if(markeds != cell(i, j).mAdjacents)
        return false;
    //True is returned if at least one cell has been uncovered.
    bool ret = false;
    for_each_nb_in_range(i, j, [this, &ret](int k, int l){ ret = uncover_if_unmarked(k, l) || ret; });
    return ret;
}

bool Minesweeper::chord_all()
{
    bool ret = false;
    for(int i = 0; i < mRows; ++i)
    for(int j = 0; j < mCols; ++j)
    {
        ret = chord(i, j) || ret;
    }
    return ret;
}

bool Minesweeper::click(int i, int j)
{
    if(cell(i, j).mVisible)
        return chord(i, j);
    else
        return uncover_if_unmarked(i, j);
}

void Minesweeper::replay()
{
    for(CellEntry& c: mData)
        c.flag = c.mVisible = false;
    mState = GameState::running;
}

void Minesweeper::reset()
{
    for(CellEntry& c: mData)
        c = CellEntry();
    mState = GameState::uninitialized;
}

void Minesweeper::p_print(std::ostream& os) const
{
    os << "   ";
    for(int j = 0; j < mCols; ++j)
        os << j % 10;
    os << '\n';
    for(int i = 0; i < mRows; ++i)
    {
        os << '\n' << std::setw(2) << i << ' ';
        for(int j = 0; j < mCols; ++j)
        {
            if(cell(i, j).mMine)
                os << '*';
            else if(cell(i, j).mAdjacents == 0)
                os << '.';
            else
                os << cell(i, j).mAdjacents;
        }
        os << ' ' << std::setw(2) << i;
    }
    os << "\n\n   ";
    for(int j = 0; j < mCols; ++j)
        os << j % 10;
}

std::ostream& operator<<(std::ostream& os, const Minesweeper& ms)
{
    os << "   ";
    for(int j = 0; j < ms.cols(); ++j)
        os << j % 10;
    os << '\n';
    for(int i = 0; i < ms.rows(); ++i)
    {
        os << '\n' << std::setw(2) << i << ' ';
        for(int j = 0; j < ms.cols(); ++j)
        {
            if(!ms.cell(i, j).visible())
            {
                if(ms.cell(i, j).flag)
                    os << 'F';
                else
                    os << '?';
            }
            else if(ms.cell(i, j).adjacents() == 0)
                os << '.';
            else
                os << ms.cell(i, j).adjacents();
        }
        os << ' ' << std::setw(2) << i;
    }
    os << "\n\n   ";
    for(int j = 0; j < ms.cols(); ++j)
        os << j % 10;

    return os;
}
