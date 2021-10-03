#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QGridLayout>
#include <QPushButton>

#include "mainfield.h"
#include "timer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    MainField* field;
    Timer* timer;
    QPushButton* reset_button;

    QHBoxLayout* top_layout;
    QVBoxLayout* mid_layout;
    QHBoxLayout* main_layout;
};
#endif // MAINWINDOW_H
