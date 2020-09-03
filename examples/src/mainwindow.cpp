#include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    QHBoxLayout* layout = new QHBoxLayout;
    auto example1 = new XB::MultiSlider(Qt::Horizontal);
    example1->addHandle(40);
    example1->addHandle(60);
    example1->setDisabled(true);
    layout->addWidget(example1);
    auto example2 = new XB::MultiSlider(Qt::Vertical);
    example2->addHandle(40);
    example2->addHandle(60);
    example2->addHandle(80);
    layout->addWidget(example2);
    QWidget* w = new QWidget;
    w->setLayout(layout);
    setCentralWidget(w);
}

MainWindow::~MainWindow()
{
}
