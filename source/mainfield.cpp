#include "mainfield.h"
#include <random>
#include <iostream>
#include <QDebug>
#include <ctime>

using namespace std;

void sleep(std::clock_t t) {
    for (std::clock_t tt = clock() + t; clock() < tt;);
}

MainField::MainField(int field_size, QWidget *parent)
    : QAbstractButton(parent)
{
    connect(this, &MainField::pressed, this, QOverload<>::of(&MainField::onPressed));
    connect(this, &MainField::released, this, QOverload<>::of(&MainField::onReleased));

    setMinimumSize(350, 350);
    QSizePolicy policy{QSizePolicy::Preferred, QSizePolicy::Preferred};
    setSizePolicy(policy);

    reset(field_size);
}

void MainField::reset(int field_size)
{
    main_field.reset(field_size, field_size, empty);
    plate_field.reset(field_size, field_size, closed);

    plantBomb();
}

QPoint MainField::pointToIndex(QPoint point)
{
    double side = qMin(width(), height());

    double col = (point.x() - (width() - side) / 2) * size() / side;
    double row = (point.y() - (height() - side) / 2) * size() / side;

    return QPoint(row, col);
}

void MainField::onPressed()
{
    mouse_index = pointToIndex(mapFromGlobal(QCursor::pos()));
    qDebug() << mouse_index;
}

void MainField::onReleased()
{
    if (mouse_index == pointToIndex(mapFromGlobal(QCursor::pos()))) {
        int col = mouse_index.x(), row = mouse_index.y();

        if (plate_field[row][col] == opened)
            return;
        if (main_field[row][col] == bomb)
            reset(size());

        clearPlate(row, col);

        update();
    }
}

void MainField::plantBomb()
{
    static constexpr int search[8][2] {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

    int field_size = size();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, field_size - 1);

    int bomb_count = field_size * field_size / 6.5;

    while (bomb_count) {
        int row = dis(gen), col = dis(gen);
        if (main_field[row][col] != bomb) {
            bomb_count--;
            main_field[row][col] = bomb;
            for (auto s : search) {
                int x = row + s[0], y = col + s[1];
                if (!out_of_range(x, y) && main_field[x][y]!= bomb)
                    main_field[x][y]++;
            }
        }
    }
//    for (int i = 0; i < field_size; i++) {
//        for (int j = 0; j < field_size; j++)
//            cout << main_field[i][j] << ' ';
//        cout << '\n';
//    }
}

void MainField::paintEvent(QPaintEvent*)
{
     // closed1, closed2, number, bomb
    static constexpr QColor box_color[3] {{130, 224, 17}, {218, 247, 166}, {255, 255, 255}};
    int color_switch = 0;
    int side = qMin(width(), height());
    double box_size = 100.0 / size();

    QPainter painter(this);
    painter.translate((width() - side) / 2, (height() - side) / 2);
    painter.scale(side / 100.0, side / 100.0);

    QPen pen;
    pen.setWidth(1);
    painter.setPen(pen);

    QFont font;
    font.setPixelSize(100.0 / size());
    painter.setFont(font);

    bool even = !(size() & 1);
    for (int row = 0; row < size(); row++) {
        for (int col = 0; col < size(); col++) {
            color_switch = !color_switch;

            QRectF rect {QPointF{row * box_size, col * box_size},
                QPointF{(row + 1) * box_size, (col + 1) * box_size}};

            if (plate_field[row][col] == opened) {
                painter.setBrush(box_color[2]);
                painter.drawRect(rect);
                if (main_field[row][col] != empty)
                    painter.drawText(rect, Qt::AlignCenter, QString::number(main_field[row][col]));
            }
            else {
                painter.setBrush(box_color[color_switch]);
                painter.drawRect(rect);
            }
        }
        if (even) color_switch = !color_switch;
    }
}

void MainField::clearPlate(int row, int col)
{
    if (out_of_range(row, col) || plate_field[row][col] == opened)
        return;

    plate_field[row][col] = opened;

    if (main_field[row][col] != empty)
        return;

    clearPlate(row + 1, col);
    clearPlate(row, col + 1);
    clearPlate(row - 1, col);
    clearPlate(row, col - 1);
}
