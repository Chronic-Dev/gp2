#ifndef GPMAINWINDOW_H
#define GPMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class GPMainWindow;
}

class GPMainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit GPMainWindow(QWidget *parent = 0);
    ~GPMainWindow();

private:
    Ui::GPMainWindow *ui;
};

#endif // GPMAINWINDOW_H
