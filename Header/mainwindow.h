#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtGui>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QProgressBar>
#include <QSpinBox>

#include "bombfield.h"
#include "timer.h"
#include "resource_paths.h"

#define FIELD_MIN_SIZE 8
#define FIELD_MAX_SIZE 50

#define DEFAULT_SIZE 10

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
    void gameEnded(BombField::EndCode);

private:
    Ui::MainWindow *ui;

    QSpinBox* bomb_count_input;
    Timer* timer;
    QPushButton* reset_button;
    BombField* field;
    QProgressBar* progress_bar;

    QHBoxLayout* top_layout;
    QVBoxLayout* mid_layout;
    QHBoxLayout* main_layout;
};
#endif // MAINWINDOW_H
