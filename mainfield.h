#ifndef MAINFIELD_H
#define MAINFIELD_H

#include <QtGui>
#include <QtCore>
#include <QAbstractButton>
#include "matrix.h"

#define BOMB_RATIO 6.5

class MainField : public QAbstractButton
{
    Q_OBJECT
public:

    enum End {
        bomb_exploded,
        mission_completed
    };

    enum Kind {
        empty = 0,
        bomb = -1,

        opened = 01,
        closed = 02,
        flagged = 04 // (closed, flagged) <-exclusive-> (opened)
    };

    explicit MainField(int field_size, const QString& flag_image_path, QWidget *parent = nullptr);
    void reset(int field_size);

    QPoint pointToIndex(QPointF point);


    inline bool out_of_range(int row, int col) {
        return qMin(row, col) < 0 || qMax(row, col) >= size();
    }

    inline int size() const { return main_field.row(); }
    inline int get_num_of_bomb() const { return num_of_bomb; }
    inline int get_num_of_opened() const { return num_of_opened; }

signals:
    void statusChanged(int);
    void end_of_game(MainField::End);

private:
    void plantBomb();
    void clearPlate(int row, int col);

    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;

    matrix<signed char> main_field; // empty, bomb, num of around bomb?
    matrix<unsigned char> plate_field; // closed, opened, flagged
    QPoint mouse_index;

    QImage flag_image;

    int num_of_bomb = 0;
    int num_of_opened = 0;
};

#endif // MAINFIELD_H
