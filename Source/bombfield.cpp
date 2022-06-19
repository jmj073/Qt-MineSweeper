#include "bombfield.h"
#include <random>
#include <iostream>
#include <QDebug>
#include <ctime>

using namespace std;

BombField::BombField(QWidget *parent, size_t field_size, const QString& flag_image_path, double bomb_ratio)
    : QAbstractButton(parent), bomb_container(field_size, field_size, bomb_ratio), flag_image(flag_image_path)
{
    assert(!flag_image.isNull());

    setMinimumSize(350, 350);
    setSizePolicy(QSizePolicy{QSizePolicy::Minimum, QSizePolicy::Minimum});

    reset();
    update();
}

void BombField::reset(size_t _field_size, double bomb_ratio)
{
    bomb_container.reset(_field_size, _field_size, bomb_ratio);

    scale = 1;
    field_pos = {0, 0};

    update();
    emit statusChanged(0);
}

void BombField::reset()
{
    bomb_container.reset();

    scale = 1;
    field_pos = {0, 0};

    emit statusChanged(0);
    update();
}

QPoint BombField::pointToIndex(QPointF point)
{
    point = pointToFieldPos(point);

    double side = qMin(width(), height());
    point *= field_size() / side / scale;

    return QPoint(point.y(), point.x());
}


void BombField::wheelEvent(QWheelEvent* event)
{
    scale *=  (event->angleDelta().y() >= 0) ? 2 : 0.5;
    field_pos -= pointToFieldPos(event->position()) * ((event->angleDelta().y() >= 0) ? 1 : -0.5);
    update();
}

void BombField::paintEvent(QPaintEvent*)
{
     // closed1, closed2, number, bomb
    static constexpr QColor box_color[3] {{130, 224, 17}, {218, 247, 166}, {255, 255, 255}};
    int side = qMin(width(), height());
    double box_size = 100.0 / field_size();

    QPainter painter(this);
//    painter.translate(field_pos);
    painter.translate(QPoint{width() - side, height() - side} / 2 + field_pos);
    painter.scale(side / 100.0 * scale, side / 100.0 * scale);

    QPen pen;
    pen.setWidth(10.0 / field_size());
    painter.setPen(pen);

    QFont font;
    font.setPixelSize(100.0 / field_size());
    painter.setFont(font);

    for (size_t row = 0; row < field_size(); row++) {
        for (size_t col = 0; col < field_size(); col++) {

            QRectF rect {QPointF{row * box_size, col * box_size},
                QPointF{(row + 1) * box_size, (col + 1) * box_size}};

            if (bomb_container.plate_grid(row, col) == BombContainer::opened) {
                painter.setBrush(box_color[2]);
                painter.drawRect(rect);
                if (bomb_container.bomb_grid(row, col) != BombContainer::empty)
                    painter.drawText(rect, Qt::AlignCenter, QString::number(bomb_container.bomb_grid(row, col)));
            }
            else if (bomb_container.plate_grid(row, col) & BombContainer::flagged)
                painter.drawImage(rect, flag_image);
            else {
                painter.setBrush(box_color[(row + col) & 1]);
                painter.drawRect(rect);
            }
        }
    }
}

void BombField::mousePressEvent(QMouseEvent* event)
{
    if (event->button() & (Qt::LeftButton | Qt::RightButton))
        mouse_pos = event->position();
}

void BombField::mouseReleaseEvent(QMouseEvent* event)
{
    keep = {0, 0};

    if (mouse_pos != event->position())
        return;

    QPoint idx = pointToIndex(event->position());

    size_t col = idx.x(), row = idx.y();
    if (bomb_container.out_of_range(row, col) || bomb_container.plate_grid(row, col) == BombContainer::opened)
        return;

    if (event->button() & Qt::LeftButton) {
        if (bomb_container.bomb_grid(row, col) == BombContainer::bomb) {
            emit end_of_game(bomb_exploded);
            return;
        }

        bomb_container.openPlate(row, col);
        emit statusChanged(100.0 * bomb_container.num_of_opened() /  (bomb_container.size() - bomb_container.num_of_bomb()));

        if (bomb_container.size() == bomb_container.num_of_opened() + bomb_container.num_of_bomb()) {
            emit end_of_game(mission_completed);
            return;
        }
    }
    else if(event->button() & Qt::RightButton)
        bomb_container.switchFlag(row, col);

    update();
}

void BombField::mouseMoveEvent(QMouseEvent* event)
{
    field_pos += event->position() - mouse_pos - keep;
    keep = event->position() - mouse_pos;
    update();
}
