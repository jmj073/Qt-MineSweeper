#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QWidget* placeholder = new QWidget(this);

    top_layout = new QHBoxLayout;
    mid_layout = new QVBoxLayout;

    bomb_count_input = new QSpinBox(placeholder);
    timer = new Timer(placeholder);
    reset_button = new QPushButton("Reset", placeholder);
    field = new BombField(placeholder, DEFAULT_SIZE, FLAG_IMAGE_PATH);
    progress_bar = new QProgressBar;

    bomb_count_input->setRange(FIELD_MIN_SIZE, FIELD_MAX_SIZE);
    bomb_count_input->setValue(DEFAULT_SIZE);

    ui->statusBar->addPermanentWidget(progress_bar);

    top_layout->addWidget(bomb_count_input);
    top_layout->addWidget(timer);
    top_layout->addWidget(reset_button);

    mid_layout->addLayout(top_layout);
    mid_layout->addWidget(field);

    placeholder->setLayout(mid_layout);
    setCentralWidget(placeholder);

    connect(reset_button, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    connect(field, &BombField::statusChanged, progress_bar, &QProgressBar::setValue);
    connect(field, &BombField::end_of_game, this, &MainWindow::gameEnded);
}

void MainWindow::onResetButtonClicked()
{
    field->reset(bomb_count_input->value());
    ui->statusBar->showMessage("reseted", 2000);
}

void MainWindow::gameEnded(BombField::EndCode endcode)
{
    if (endcode == BombField::EndCode::bomb_exploded)
        ui->statusBar->showMessage("explosion!!!!!", 2000);
    else if (endcode == BombField::EndCode::mission_completed)
        ui->statusBar->showMessage("mission complete!!", 2000);
    field->reset(bomb_count_input->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

