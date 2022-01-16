#include "bombcontainer.h"

#include <random>

using namespace std;

BombContainer::BombContainer(size_t row_size, size_t col_size, double _bomb_ratio)
    : m_bomb_grid(row_size, col_size, empty), m_plate_grid(row_size, col_size, closed),
      m_bomb_ratio{ (_bomb_ratio > 1) ? 1 : ((_bomb_ratio < 0) ? 0 : _bomb_ratio) },
      m_num_of_bomb(size() * _bomb_ratio)
{
    plantBomb();
}

void BombContainer::reset(size_t row_size, size_t col_size, double _bomb_ratio)
{
    if (m_bomb_grid.reshape(row_size, col_size)) {
        m_plate_grid.reshape(row_size, col_size);
        std::fill(m_bomb_grid.begin(), m_bomb_grid.end(), empty);
        std::fill(m_plate_grid.begin(), m_plate_grid.end(), closed);
    }
    else {
        m_bomb_grid = Matrix<StateType>(row_size, col_size, empty);
        m_plate_grid = Matrix<StateType>(row_size, col_size, closed);
    }

    m_bomb_ratio = (_bomb_ratio > 1) ? 1 : ((_bomb_ratio < 0) ? 0 : _bomb_ratio);
    m_num_of_bomb = size() * m_bomb_ratio;
    m_num_of_opened = 0;

    plantBomb();
}

void BombContainer::reset()
{
    m_num_of_opened = 0;

    std::fill(m_bomb_grid.begin(), m_bomb_grid.end(), empty);
    std::fill(m_plate_grid.begin(), m_plate_grid.end(), closed);

    plantBomb();
}

void BombContainer::openPlate(size_t row, size_t col)
{
    QPoint stack[2500]; // magic literal 2500
    QPoint* sp = stack;
    QPoint cur; // currrent

    *sp++ = QPoint(row, col);
    while(sp - stack) {
        cur = *--sp;
        if (out_of_range(cur.x(), cur.y()) || m_plate_grid[cur.x()][cur.y()] == opened)
            continue;

        m_plate_grid[cur.x()][cur.y()] = opened;
        ++m_num_of_opened;
//        repaint();

        if (m_bomb_grid[cur.x()][cur.y()] != empty)
            continue;

        *sp++ = QPoint{cur.x(), cur.y() + 1};
        *sp++ = QPoint{cur.x(), cur.y() - 1};
        *sp++ = QPoint{cur.x() + 1, cur.y()};
        *sp++ = QPoint{cur.x() - 1, cur.y()};
        *sp++ = QPoint{cur.x() + 1, cur.y() + 1};
        *sp++ = QPoint{cur.x() - 1, cur.y() - 1};
        *sp++ = QPoint{cur.x() + 1, cur.y() - 1};
        *sp++ = QPoint{cur.x() - 1, cur.y() + 1};
    }
}

void BombContainer::plantBomb()
{
    static constexpr int search[8][2] {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

    static random_device rd;

    mt19937 gen(rd());
    uniform_int_distribution<size_t> r_dis(0, row() - 1);
    uniform_int_distribution<size_t> c_dis(0, col() - 1);

    size_t bomb_count = m_num_of_bomb;
    while (bomb_count > 0) {
        int row = r_dis(gen), col = c_dis(gen);
        if (m_bomb_grid[row][col] != bomb) {
            bomb_count--;
            m_bomb_grid[row][col] = bomb;
            for (auto s : search) {
                size_t x = row + s[0], y = col + s[1];
                if (!out_of_range(x, y) && m_bomb_grid[x][y] != bomb)
                    m_bomb_grid[x][y]++;
            }
        }
    }
    qDebug() << "-------------------------";
}

















