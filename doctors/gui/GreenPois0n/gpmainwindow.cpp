#include "gpmainwindow.h"
#include "ui_gpmainwindow.h"

GPMainWindow::GPMainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GPMainWindow)
{
    ui->setupUi(this);
}

GPMainWindow::~GPMainWindow()
{
    delete ui;
}
