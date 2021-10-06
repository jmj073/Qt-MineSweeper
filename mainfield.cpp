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

//    explosion_shower = new QVideoWidget(this);
//    explosion_shower->hide();

//    explosion_player = new QMediaPlayer(this);
//    explosion_player->setSource(QUrl("qrc:/src/explosion.mp4"));
//    explosion_player->setVideoOutput(explosion_shower);

    reset(field_size);
}

void MainField::reset(int field_size)
{
    main_field.reset(field_size, field_size, empty);
    plate_field.reset(field_size, field_size, closed);

    update();
    plantBomb();
}

QPoint MainField::pointToIndex(QPointF point)
{
    double side = qMin(width(), height());

    point -= QPointF(width() - side, height() - side) / 2;
    point *= size() / side;

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
//    for (int i = 0; i < field_size; i++) {
//        for (int j = 0; j < field_size; j++)
//            cout << main_field[i][j] << ' ';
//        cout << '\n';
//    }
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
    painter.translate((width() - side) / 2, (height() - side) / 2);
    painter.scale(side / 100.0, side / 100.0);

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
        mouse_index = pointToIndex(mapFromGlobal(event->globalPosition()));
}

void MainField::mouseReleaseEvent(QMouseEvent* event)
{
    if (mouse_index != pointToIndex(mapFromGlobal(QCursor::pos())))
        return;
    int col = mouse_index.x(), row = mouse_index.y();
    if (out_of_range(row, col) || plate_field[row][col] & opened)
        return;

    if (event->button() & Qt::LeftButton) {
        if (main_field[row][col] == bomb) {
        //            show_explosion();
            reset(size());
            return;
        }
        clearPlate(row, col);
    }
    else if(event->button() & Qt::RightButton)
        plate_field[row][col] ^= flagged;

    update();
}


//void MainField::show_explosion()
//{
//    explosion_shower->show();
//    explosion_player->play();
////    explosion_shower->hide();
//}
