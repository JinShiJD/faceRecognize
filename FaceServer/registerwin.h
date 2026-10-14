#ifndef REGISTERWIN_H
#define REGISTERWIN_H

#include <QWidget>
#include <opencv.hpp>


namespace Ui {
class registerWin;
}

class registerWin : public QWidget
{
    Q_OBJECT

public:
    explicit registerWin(QWidget *parent = nullptr);
    ~registerWin();

    void timerEvent(QTimerEvent *e);

private slots:
    void on_btnReset_clicked();

    void on_btnAddHeader_clicked();

    void on_btnAdd_clicked();

    void on_btnOpenCamera_clicked();

    void on_btnPhotograph_clicked();

private:
    Ui::registerWin *ui;

    int timerid;
    cv::VideoCapture cap;
    cv::Mat img;
};

#endif // REGISTERWIN_H
