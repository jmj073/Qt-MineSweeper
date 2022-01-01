#ifndef BOMBCONTAINER_H
#define BOMBCONTAINER_H

#include <QtCore>
#include <stdint.h>
#include "Matrix.h"

class BombContainer
{
public:
    using StateType = int8_t;

    enum BombState : StateType {
        empty = 0,
        bomb = -1
    };
    enum PlateState : StateType {
        opened = 01,
        closed = 02,
        flagged = 04 // (closed, flagged) <-exclusive-> (opened)
    };

public:
    BombContainer(size_t row_size, size_t col_size, double _bomb_ratio = 0.2);
    BombContainer(size_t size, double _bomb_ratio = 0.2);

    void reset(size_t row_size, size_t col_size, double _bomb_ratio = 0.2);
    void reset();

    size_t size() const { return m_bomb_grid.size(); }
    size_t row() const { return m_bomb_grid.row(); }
    size_t col() const { return m_bomb_grid.col(); }
    size_t num_of_bomb() const { return m_num_of_bomb; }
    double bomb_ratio() const { return m_bomb_ratio; }
    size_t num_of_opened() const { return m_num_of_opened; }
    bool out_of_range(size_t _row, size_t _col) // unsigned라 가정
    {
        static_assert (size_t(-1) >= 0, "size_t is not unsigned");
        return _row >= row() || _col >= col();
    }

    StateType bomb_grid(size_t row, size_t col) const
    { return m_bomb_grid[row][col]; }
    StateType plate_grid(size_t row, size_t col) const
    { return m_plate_grid[row][col]; }

    void openPlate(size_t row_idx, size_t col_idx);
    void switchFlag(size_t row_idx, size_t col_idx)
    {
        m_plate_grid[row_idx][col_idx] ^= flagged;
    }

private:
    void plantBomb();

private:
    // 아래 둘은 (row, col)이 같아야함
    Matrix<StateType> m_bomb_grid; // empty, bomb, num of around bomb?
    Matrix<StateType> m_plate_grid; // closed, opened, flagged

    double m_bomb_ratio;
    size_t m_num_of_opened;
    size_t m_num_of_bomb;
};

#endif // BOMBCONTAINER_H
