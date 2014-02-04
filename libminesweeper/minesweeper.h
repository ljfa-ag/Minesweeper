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

#ifndef MINESWEEPER_H_INCLUDED
#define MINESWEEPER_H_INCLUDED

#include <iostream>
#include <stdexcept>
#include <vector>

#include <boost/random/uniform_smallint.hpp>

/** \brief Class for representing a Minesweeper game state */
class Minesweeper
{
public:
    ///State of a cell
    class CellEntry
    {
    public:
        ///Cell is flagged by the player, i.e. he supposes a mine there
        bool flag;
        ///Returns if the cell is uncovered
        bool visible() const { return mVisible; }
        ///Returns the number of mines in the adjacent cells if this cell is visible, or -1 otherwise
        int adjacents() const;

        ///Returns if the cell contains a mine
        bool p_mine() const { return mMine; }
        ///Returns the number of mines in the adjacent cells
        int p_adjacents() const { return mAdjacents; }

    private:
        bool mMine;
        bool mVisible;
        int mAdjacents;

        explicit CellEntry(bool mine = false, bool visible = false, int adjacents = -1, bool flag = false);

        friend class Minesweeper;
    };

    ///State of the game
    enum class GameState
    {
        ///The game has not been \ref init'ed
        uninitialized,
        ///The game is running
        running,
        ///The player won
        win,
        ///The player lost
        loss
    };

    /// \throw std::out_of_range if rows or cols is negative
    Minesweeper(int rows, int cols);

    /** \brief Initializes the minefield
     * \param mines The number of mines
     * \param rng A boost::random generator
     * \param startr The row of the starting position
     * \param startc The column of the starting position
     * \throw std::out_of_range if mines >= cells()
     *
     * The specified number of mines is randomly placed across the field,
     * but the cell (\c startr, \c startc) is left open.
     */
    template<class RNG> void init(unsigned int mines, RNG& rng, int startr = -1, int startc = -1);

    ///Returns the number of rows
    int rows() const { return mRows; }
    ///Returns the number of columns
    int cols() const { return mCols; }
    ///Returns the number of cells
    unsigned int cells() const { return mRows*mCols; }
    ///Checks if (\c i, \c j) is in range of the field
    bool in_range(int i, int j) const;

    ///Returns the current state of the game
    GameState state() const { return mState; }

    ///Returns if the game is in progress
    bool running() const { return mState == GameState::running; }

    /** \brief Returns a reference to the cell (\c i, \c j).
     *
     * No range check is done.
     */
    CellEntry& cell(int i, int j);

    /** \brief Returns a reference to the cell (\c i, \c j).
     *
     * No range check is done.
     */
    const CellEntry& cell(int i, int j) const;

    /// Returns the cell (\c i, \c j) if it is in range, or an empty cell (i.e. \c CellEntry()) otherwise
    CellEntry try_get_cell(int i, int j) const;

    /** \brief Makes a move at (\c i, \c j).
     * \return \c true if a field has been uncovered
     */
    bool uncover(int i, int j);

    /** \brief Makes a move at (\c i, \c j) if the cell is unmarked
     * \return \c true if a field has been uncovered
     */
    bool uncover_if_unmarked(int i, int j);

    /** \brief Uncovers (i.e. makes a move) the fields around (\c i, \c j)
     * \return \c true if a field has been uncovered
     *
     * If the number of mines around (\c i, \c j) equals the number of marked cells
     * around this cell, then the unmarked fields around it are uncovered.
     * This requires (\c i, \c j) to already be uncovered.
     */
    bool uncover_around(int i, int j);

    /** \brief Calls uncover_if_unmarked() if (\c i, \c j) is visible and uncover_around() else
     * \return \c true if a field has been uncovered
     */
    bool click(int i, int j);

    ///Covers and unmarks all cells. The mines are left as they were.
    void replay();

    ///Resets the game into the uninitialized state
    void reset();

    ///Prints out the field, including the covered cells
    void p_print(std::ostream& os) const;

    ///Calls \c f for each neighbor of (\c i, \c j) in the field
    template<class Func> void for_each_nb_in_range(int i, int j, Func f);

    /** \brief Calls \c f for each neighbor of (\c i, \c j) including those outside of the field
     * \note this is a static function unlike for_each_nb_in_range().
     */
    template<class Func> static void for_each_nb(int i, int j, Func f);

private:
    int mRows, mCols;
    std::vector<CellEntry> mData;
    GameState mState;
    ///The number of covered fields
    unsigned int mCovered;

    ///Recursively uncovers (\c i, \c j) and its neigbors if the number of adjacents is 0.
    void p_rec_uncover(int i, int j);

    /** \brief Uncovers the cell (\c i, \c j).
     * \return \c true if the cell was not covered before.
     */
    bool p_uncover(int i, int j);
};

std::ostream& operator<<(std::ostream& os, const Minesweeper& ms);

template<class RNG> void Minesweeper::init(unsigned int mines, RNG& rng, int startr, int startc)
{
    if(mines >= cells())
        throw std::out_of_range("The number of mines must be smaller than the number of cells");
    if(mState != GameState::uninitialized)
        throw std::runtime_error("The field has already been initialized");

    mCovered = cells() - mines;

    boost::random::uniform_smallint<int> rdist(0, mRows-1), cdist(0, mCols-1);
    for(; mines > 0; --mines)
    {
        int i, j;
        //Don't place any mines at the starting position, and don't place double mines
        do
        {
            i = rdist(rng);
            j = cdist(rng);
        } while((i == startr && j == startc) || cell(i, j).mMine);
        cell(i, j).mMine = true;
    }
    for(int i = 0; i < mRows; ++i)
    for(int j = 0; j < mCols; ++j)
    {
        if(cell(i, j).mMine)
            continue;
        //Compute the number of mines in the neighbor fields
        int adj = 0;
        for_each_nb_in_range(i, j, [&](int k, int l)
            { adj += try_get_cell(k, l).mMine; });
        cell(i, j).mAdjacents = adj;
    }
    mState = GameState::running;
}

template<class Func> void Minesweeper::for_each_nb_in_range(int i, int j, Func f)
{
    for_each_nb(i, j, [&](int k, int l)
    {
        if(in_range(k, l))
            f(k, l);
    });
}

template<class Func> void Minesweeper::for_each_nb(int i, int j, Func f)
{
    f(i-1, j-1); f(i-1, j  ); f(i-1, j+1);
    f(i  , j-1);              f(i  , j+1);
    f(i+1, j-1); f(i+1, j  ); f(i+1, j+1);
}

#endif
