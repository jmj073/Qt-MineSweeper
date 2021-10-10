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

    bomb_count_in = new QSpinBox(placeholder);
    timer = new Timer(placeholder);
    reset_button = new QPushButton("Reset", placeholder);
    field = new MainField(DEFAULT_SIZE, FLAG_IMAGE_PATH, placeholder);
    progress_bar = new QProgressBar;

    bomb_count_in->setRange(FIELD_MIN_SIZE, FIELD_MAX_SIZE);
    bomb_count_in->setValue(DEFAULT_SIZE);

    ui->statusBar->addPermanentWidget(progress_bar);

    top_layout->addWidget(bomb_count_in);
    top_layout->addWidget(timer);
    top_layout->addWidget(reset_button);

    mid_layout->addLayout(top_layout);
    mid_layout->addWidget(field);

    placeholder->setLayout(mid_layout);
    setCentralWidget(placeholder);

    connect(reset_button, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
    connect(field, &MainField::statusChanged, progress_bar, &QProgressBar::setValue);
    connect(field, &MainField::end_of_game, this, &MainWindow::gameEnded);
}

void MainWindow::onResetButtonClicked() // *learn QRegularExpression*
{
    field->reset(bomb_count_in->value());
    ui->statusBar->showMessage("reseted", 2000);
}

void MainWindow::gameEnded(MainField::End end)
{
    if (end == MainField::End::bomb_exploded)
        ui->statusBar->showMessage("explosion!!!!!", 2000);
    else if (end == MainField::End::mission_completed)
        ui->statusBar->showMessage("mission complete!!", 2000);
    field->reset(bomb_count_in->value());
}

MainWindow::~MainWindow()
{
    delete ui;
}

