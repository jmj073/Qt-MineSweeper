#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>

#include "mainfield.h"
#include "timer.h"

#define FIELD_MIN_SIZE 8
#define FIELD_MAX_SIZE 50

#define DEFAULT_SIZE 10

#define FIELD_RANGE_MESSAGE "8 ~ 50"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onResetButtonClicked();

private:
    Ui::MainWindow *ui;

    QLineEdit* bomb_count_in;
    Timer* timer;
    QPushButton* reset_button;
    MainField* field;

    QHBoxLayout* top_layout;
    QVBoxLayout* mid_layout;
    QHBoxLayout* main_layout;
};
#endif // MAINWINDOW_H
