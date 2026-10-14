#ifndef FACEATTANDANCE_H
#define FACEATTANDANCE_H

#include <QMainWindow>
#include <opencv.hpp>
#include <QTcpSocket>
#include <QTimer>

using namespace cv;
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class FaceAttandance; }
QT_END_NAMESPACE

class FaceAttandance : public QMainWindow
{
    Q_OBJECT

public:
    FaceAttandance(QWidget *parent = nullptr);
    ~FaceAttandance();

    // 定时器事件
    void timerEvent(QTimerEvent *e);

private slots:
    void timer_connect();
    void stop_connect();
    void start_connect();

    void recvData();

private:
    Ui::FaceAttandance *ui;

    // 摄像头
    VideoCapture cap;
    // haar--级联分类器
    CascadeClassifier cascade;


    // 创建网络套接字、定时器
    QTcpSocket msocket;
    QTimer mtimer;

    // 标志是否是同一个人脸进入到识别区域
    int flag;

    // 保存人脸的数据
    cv::Mat faceMat;

    // 缓冲区
    QByteArray buf;
};
#endif // FACEATTANDANCE_H
