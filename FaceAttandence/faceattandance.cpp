#include "faceattandance.h"
#include "ui_faceattandance.h"
#include <QDebug>
#include <QImage>
#include <QPointer>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>

#include <QBitmap>
#include <QDir>
#include <QImageReader>
#include <QPainter>

FaceAttandance::FaceAttandance(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::FaceAttandance)
{
    ui->setupUi(this);

    // 打开摄像头
    cap.open(0);   // dev/video
    //启动定时器事件
    startTimer(100);

    // 导入级联分类器文件
    cascade.load("D:/OpenCV/4.5.2/opencv_install/etc/haarcascades/haarcascade_frontalface_alt2.xml");

    // 当QTcpSocket断开连接是发送disconnect信号，连接成功会发送connect信号
    connect(&msocket, &QTcpSocket::disconnected, this, &FaceAttandance::start_connect);
    connect(&msocket, &QTcpSocket::connected, this, &FaceAttandance::stop_connect);

    //关联接收数据的槽函数
    connect(&msocket, &QTcpSocket::readyRead, this, &FaceAttandance::recvData);

    // 定时器连接服务器
    connect(&mtimer, &QTimer::timeout, this, &FaceAttandance::timer_connect);
    // 启动定时器
    mtimer.start(5000);

    flag = 0;
    ui->widgetRecognize->hide();
}

FaceAttandance::~FaceAttandance()
{
    delete ui;
}

void FaceAttandance::timerEvent(QTimerEvent *e)
{
    // 采集数据
    Mat srcImage;
    if(cap.grab())
    {
        cap.read(srcImage); // 读取一帧数据
    }

    // 把图片大小设置与显示窗口一样大小
    cv::resize(srcImage, srcImage, Size(480, 480));

    Mat grayImg;
    // 转为灰度图(加快检测速度)
    cvtColor(srcImage, grayImg, COLOR_BGR2GRAY);
    // 检测人脸数据
    std::vector<Rect> faceRects;
    cascade.detectMultiScale(grayImg, faceRects);
    if(faceRects.size() > 0 && flag >= 0)
    {
        Rect rect = faceRects.at(0); // 第一个人脸的矩形框
        //rectangle(srcImage, rect, Scalar(0, 0, 186)); //矩形框

        // 移动人脸框
        ui->labHeader->move(rect.x, rect.y);

        //opencv编码人脸图片信息
        // 把Mat数据转换为QByteArray ==>> 编码为jpg格式
        if(flag > 2)
        {
            std::vector<uchar> buf;
            cv::imencode(".jpg", srcImage, buf);
            QByteArray byte((const char*)buf.data(), buf.size());
            // 准备发送
            quint64 backSize = byte.size();
            QByteArray sendData;
            QDataStream stream(&sendData, QIODevice::WriteOnly);
            stream.setVersion(QDataStream::Qt_5_14);
            stream << backSize << byte;
            // 发送
            msocket.write(sendData);
            flag = -2;

            //faceMat = srcImage(rect);
            // 保存
            //imwrite("./face.jpg", faceMat);


            faceMat = srcImage(rect).clone();

            // 保存图片，并检查返回值
            std::string savePath = QDir::currentPath().toStdString() + "/face.png";
            bool success = cv::imwrite(savePath, faceMat);
            if (success) {
                qDebug() << "人脸图片保存成功：" << QString::fromStdString(savePath);
            } else {
                qDebug() << "人脸图片保存失败！请检查路径权限或磁盘空间。";
            }

        }
        flag ++;
    }
    if(faceRects.size() == 0)
    {
        // 把识别图像框移动至中间位置
        ui->labHeader->move(100, 60);
        ui->widgetRecognize->hide();
        flag = 0;
    }


    if(srcImage.data == nullptr)   return;
    // 把opencv里面的Mat颜色数据(BGR)转Qt里面的QImage(RGB)
    cvtColor(srcImage, srcImage, COLOR_BGR2RGB);
    QImage image(srcImage.data, srcImage.cols, srcImage.rows, srcImage.step1(), QImage::Format_RGB888);
    QPixmap pmp = QPixmap::fromImage(image);
    ui->labVideo->setPixmap(pmp);

}

void FaceAttandance::timer_connect()
{
    // 连接服务器
    if(msocket.state() == QAbstractSocket::UnconnectedState)
    {
       // msocket.connectToHost("192.168.3.48", 9999);
         msocket.connectToHost("127.0.0.1", 9999);
        qDebug() << "正在连接服务器..." ;
    }
}

void FaceAttandance::stop_connect()
{
    mtimer.stop();
    qDebug() << "成功连接服务器";
}

void FaceAttandance::start_connect()
{
    mtimer.start(5000); // 启动定时器
    qDebug() << "断开连接";
}

//void FaceAttandance::recvData()
//{
//    // {employeeId:%1, name:%2, department:软件,time:%3}
//    QByteArray array = msocket.readAll();
//    // Json解析
//    QJsonParseError err;
//    QJsonDocument doc = QJsonDocument::fromJson(array, &err);
//    if(err.error != QJsonParseError::NoError)
//    {
//        qDebug() << "Json 数据错误";
//        qDebug() <<  "array: " << array;
//        return;
//    }

//    QJsonObject obj = doc.object();
//    QString employeeId = obj.value("employeeId").toString();
//    QString name = obj.value("name").toString();
//    QString department = obj.value("department").toString();
//    QString timeStamp = obj.value("time").toString();

//    ui->lineEditEmployeeId->setText(employeeId);
//    ui->lineEditName->setText(name);
//    ui->lineEditDepartment->setText(department);
//    ui->lineEditTime->setText(timeStamp);

//    // 通过样式来显示图片
//    ui->labImgHeader->setStyleSheet("border-radius:75px;border-image:url(./face.jpg);");
//    ui->widgetRecognize->show();
//}
void FaceAttandance::recvData()
{
    QByteArray array = msocket.readAll();
    QList<QByteArray> lines = array.split('\n');
    for (const QByteArray &line : lines)
    {
        if (line.trimmed().isEmpty()) continue;
        QJsonParseError err;
        QJsonDocument doc = QJsonDocument::fromJson(line, &err);
        if (err.error != QJsonParseError::NoError) {
            qDebug() << "Json 数据错误:" << line;
            continue;
        }
        QJsonObject obj = doc.object();
        QString employeeId = obj.value("employeeId").toString();
        QString name = obj.value("name").toString();
        QString department = obj.value("department").toString();
        QString timeStamp = obj.value("time").toString();
        QString headFile = obj.value("headfile").toString();

        qDebug().noquote() << "01--headFile: " << headFile;

        ui->lineEditEmployeeId->setText(employeeId);
        ui->lineEditName->setText(name);
        ui->lineEditDepartment->setText(department);
        ui->lineEditTime->setText(timeStamp);
        qDebug().noquote() << "02--headFile: " << headFile;
        qDebug() << "当前文件位置：" << QDir::currentPath();
        //QString fullPath = QDir::currentPath() + headFile;

        //ui->labImgHeader->setStyleSheet("border-radius:75px;border-image:url(./face.jpg);");
        //ui->labImgHeader->setStyleSheet(QString("border-image: url(:\"%1\");border-radius:75px;").arg(headFile));
        //ui->labImgHeader->setStyleSheet(QString("image: url(\"%1\");border-radius:75px;").arg(fullPath));

        QString exeDir = QCoreApplication::applicationDirPath();
        QString fullPath = exeDir + "/face.png";

        // 诊断信息
        QFileInfo fi(fullPath);
        qDebug() << "尝试加载图片:" << fi.absoluteFilePath();
        qDebug() << "文件存在:" << fi.exists() << "大小:" << fi.size();

        // 尝试用 OpenCV 读取验证（可选）
        cv::Mat test = cv::imread(fullPath.toStdString());
        if (test.empty()) {
            qDebug() << "警告：OpenCV 也无法读取该图片文件";
        }


        QPixmap pixmap(fullPath);

        // 诊断信息
        if (pixmap.isNull()) {
            qDebug() << "图片加载失败：" << fullPath;
            // 可以尝试用 QImageReader 获取详细错误
            QImageReader reader(fullPath);
            qDebug() << "错误详情：" << reader.errorString();
        }

        if (!pixmap.isNull()) {
            // 缩放至标签合适大小（假设标签大小为 150x150）
            QPixmap scaled = pixmap.scaled(150, 150, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            // 裁剪成圆形（可选）
            QPixmap rounded = scaled;
            QBitmap mask(rounded.size());
            mask.fill(Qt::color0);
            QPainter painter(&mask);
            painter.setBrush(Qt::color1);
            painter.setRenderHint(QPainter::Antialiasing);
            painter.drawEllipse(0, 0, rounded.width(), rounded.height());
            rounded.setMask(mask);

            ui->labImgHeader->setPixmap(rounded);
        } else {
            qDebug() << "图片加载失败：" << fullPath;
        }

        // 仅设置一次样式表（圆角已由mask处理，此处仅保证背景透明等）
        ui->labImgHeader->setStyleSheet("border-radius:75px; background: transparent;");

        ui->widgetRecognize->show();
    }

}
