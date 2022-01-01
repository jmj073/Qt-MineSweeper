#ifndef BOMBFIELD_H
#define BOMBFIELD_H

#include <QtGui>
#include <QtCore>
#include <QAbstractButton>
#include "bombcontainer.h"

//#define BOMB_RATIO 6.5

class BombField : public QAbstractButton
{
    Q_OBJECT
public:
    enum EndCode {
        bomb_exploded,
        mission_completed
    };

    BombField(QWidget *parent, size_t field_size, const QString& flag_image_path, double bomb_ratio = 0.2);
    BombField(size_t field_size, const QString& flag_image_path, double bomb_ratio = 0.2, QWidget *parent = nullptr)
        : BombField(parent, field_size, flag_image_path, bomb_ratio) {}

    size_t field_size() const { return bomb_container.row(); }


    void reset(size_t field_size, double _bomb_ratio = 0.2);
    void reset();

signals:
    void statusChanged(int);
    void end_of_game(BombField::EndCode);

protected:
    void wheelEvent(QWheelEvent*) override;
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;

    QPointF pointToFieldPos(QPointF point); // 폭탄 격자 기준으로 변환
    QPoint pointToIndex(QPointF point); // pointToFieldPos에서 구한 좌표를 인덱스로 변환

private:
    BombContainer bomb_container;

    QPointF field_pos;
    QPointF mouse_pos;
    QPointF keep;

    QImage flag_image;

    double scale = 1;
};

inline QPointF BombField::pointToFieldPos(QPointF point)
{
    double side = qMin(width(), height());
    return point - (QPointF(width() - side, height() - side) / 2 + field_pos);
}

#endif // BOMBFIELD_H
