#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_pressed();
    void on_tableWidget_cellChanged(int row, int column);
    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();
    void handleError(int ,int ,int);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
