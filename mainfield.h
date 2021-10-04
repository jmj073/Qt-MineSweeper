#ifndef MAINFIELD_H
#define MAINFIELD_H

#include <QtGui>
#include <QtCore>
#include <QAbstractButton>
#include <QMediaPlayer>
#include <QVideoWidget>
#include "matrix.h"

class MainField : public QAbstractButton
{
    Q_OBJECT
public:

    enum Kind {
        empty = 0,
        bomb = -1,
        closed = false,
        opened = true
    };

    explicit MainField(int field_size, QWidget *parent = nullptr);
    void reset(int field_size);

    inline int size() const {
        return main_field.row();
    }
    QPoint pointToIndex(QPoint point);

    inline bool out_of_range(int row, int col) {
        return qMin(row, col) < 0 || qMax(row, col) >= size();
    }

private slots:
    void onPressed();
    void onReleased();

private:
    void plantBomb();
    void paintEvent(QPaintEvent*) override;
    void clearPlate(int row, int col);
    // test
    void show_explosion();

    matrix<int> main_field;
    matrix<bool> plate_field;
    QPoint mouse_index;
    QMediaPlayer* explosion_player;
    QVideoWidget* explosion_shower;
};

#endif // MAINFIELD_H
