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

    bomb_count_in = new QLineEdit(QString::number(DEFAULT_SIZE), placeholder);
    timer = new Timer(placeholder);
    reset_button = new QPushButton("reset", placeholder);
    field = new MainField(DEFAULT_SIZE, placeholder);

    top_layout->addWidget(bomb_count_in);
    top_layout->addWidget(timer);
    top_layout->addWidget(reset_button);

    mid_layout->addLayout(top_layout);
    mid_layout->addWidget(field);

    placeholder->setLayout(mid_layout);
    setCentralWidget(placeholder);

    connect(reset_button, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);
}

void MainWindow::onResetButtonClicked() // *learn QRegularExpression*
{
    auto list = bomb_count_in->text().split(QRegularExpression("\\D+"), Qt::SkipEmptyParts);

    if (list.empty()) {
        bomb_count_in->setText(FIELD_RANGE_MESSAGE);
        return;
    }

    int input_num = list[0].toInt();
    if (input_num < FIELD_MIN_SIZE || input_num > FIELD_MAX_SIZE) {
        bomb_count_in->setText(FIELD_RANGE_MESSAGE);
        return;
    }

    field->reset(input_num);
}

MainWindow::~MainWindow()
{
    delete ui;
}

