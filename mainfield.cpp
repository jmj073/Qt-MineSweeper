#include "mainfield.h"
#include <random>
#include <iostream>
#include <QDebug>
#include <ctime>

using namespace std;

void sleep(std::clock_t t) {
    for (std::clock_t tt = clock() + t; clock() < tt;);
}

MainField::MainField(int field_size, const QString& flag_image_path, QWidget *parent)
    : QAbstractButton(parent), flag_image(flag_image_path)
{
    assert(!flag_image.isNull());

    setMinimumSize(350, 350);
    QSizePolicy policy{QSizePolicy::Preferred, QSizePolicy::Preferred};
    setSizePolicy(policy);

    reset(field_size);
    update();
}

void MainField::reset(int field_size)
{
    main_field.reset(field_size, field_size, empty);
    plate_field.reset(field_size, field_size, closed);

    num_of_bomb = num_of_opened = 0;
    scale = 1;

    field_pos = {0, 0};

    update();
    emit statusChanged(0);
    plantBomb();
}

QPoint MainField::pointToIndex(QPointF point)
{
    double side = qMin(width(), height());

    point -= QPointF(width() - side, height() - side) / 2 + field_pos;
    qDebug() << point;
    point *= size() / side / scale;

    return QPoint(point.y(), point.x());
}

void MainField::plantBomb()
{
    static constexpr int search[8][2] {{-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1}, {1, 0}, {1, -1}, {0, -1}};

    int field_size = size();

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> dis(0, field_size - 1);

    int bomb_count = field_size * field_size / BOMB_RATIO;
    num_of_bomb = bomb_count;

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
}

void MainField::clearPlate(int row, int col)
{
    QPoint stack[2500]; // magic literal
    QPoint* sp = stack;
    QPoint cur; // currrent

    *sp++ = QPoint{row, col};
    while(sp - stack) {
        cur = *--sp;
        if (out_of_range(cur.x(), cur.y()) || plate_field[cur.x()][cur.y()] == opened)
            continue;

        plate_field[cur.x()][cur.y()] = opened;
        ++num_of_opened;
//        repaint();

        if (main_field[cur.x()][cur.y()] != empty)
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
    emit statusChanged(100 * get_num_of_opened() / ((size() * size()) - get_num_of_bomb()));
}

void MainField::wheelEvent(QWheelEvent* event)
{
    scale *=  (event->angleDelta().y() >= 0) ? 2 : 0.5;
    field_pos -= pointToFieldPos(event->position()) * ((event->angleDelta().y() >= 0) ? 1 : -0.5);
    update();
}

void MainField::paintEvent(QPaintEvent*)
{
     // closed1, closed2, number, bomb
    static constexpr QColor box_color[3] {{130, 224, 17}, {218, 247, 166}, {255, 255, 255}};
    int color_switch = 0;
    int side = qMin(width(), height());
    double box_size = 100.0 / size();

    QPainter painter(this);
    painter.translate(QPoint{width() - side, height() - side} / 2 + field_pos);
    painter.scale(side / 100.0 * scale, side / 100.0 * scale);

    QPen pen;
    pen.setWidth(10.0 / size());
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

            if (plate_field[row][col] & opened) {
                painter.setBrush(box_color[2]);
                painter.drawRect(rect);
                if (main_field[row][col] != empty)
                    painter.drawText(rect, Qt::AlignCenter, QString::number(main_field[row][col]));
            }
            else if (plate_field[row][col] & flagged)
                painter.drawImage(rect, flag_image);
            else {
                painter.setBrush(box_color[color_switch]);
                painter.drawRect(rect);
            }
        }
        if (even) color_switch = !color_switch;
    }
}

void MainField::mousePressEvent(QMouseEvent* event)
{
    if (event->button() & (Qt::LeftButton | Qt::RightButton))
        mouse_pos = event->position();
}

void MainField::mouseReleaseEvent(QMouseEvent* event)
{
    keep = {0, 0};
    if (mouse_pos != event->position())
        return;

    QPoint idx = pointToIndex(event->position());

    int col = idx.x(), row = idx.y();
    if (out_of_range(row, col) || plate_field[row][col] & opened)
        return;

    if (event->button() & Qt::LeftButton) {
        if (main_field[row][col] == bomb) {
            emit end_of_game(bomb_exploded);
//            reset(size());
            return;
        }

        clearPlate(row, col);
        if (size() * size() == num_of_opened + get_num_of_bomb()) {
            emit end_of_game(mission_completed);
            reset(size());
        }
    }
    else if(event->button() & Qt::RightButton)
        plate_field[row][col] ^= flagged;

    update();
}

void MainField::mouseMoveEvent(QMouseEvent* event)
{
    field_pos += event->position() - mouse_pos - keep;
    keep = event->position() - mouse_pos;
    update();
}
