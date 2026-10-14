#include "registerwin.h"
#include "ui_registerwin.h"
#include "facemodel.h"

#include <QFileDialog>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QMessageBox>

registerWin::registerWin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::registerWin)
{
    ui->setupUi(this);
}

registerWin::~registerWin()
{
    delete ui;
}

void registerWin::timerEvent(QTimerEvent *e)
{
    // 获取摄像头数据并显示在界面上
    if(cap.isOpened())
    {
        cap >> img;
        if(img.data == nullptr) return;
    }
    //Mat ==>> QImage
    cv::Mat rgbImg;
    cv::cvtColor(img, rgbImg, cv::COLOR_BGR2RGB);
    QImage qImg(rgbImg.data, rgbImg.cols, rgbImg.rows, rgbImg.step1(), QImage::Format_RGB888);
    // 在qt界面上显示
    QPixmap pmp = QPixmap::fromImage(qImg);
    pmp = pmp.scaledToWidth(ui->labHeader->width());
    ui->labHeader->setPixmap(pmp);

}

void registerWin::on_btnReset_clicked()
{
    //清空数据
    ui->lineEditName->clear();
    ui->dateEditBirth->setDate(QDate::currentDate());
    ui->lineEditAddr->clear();
    ui->lineEditPhone->clear();
    ui->lineEditSavePath->clear();

}

void registerWin::on_btnAddHeader_clicked()
{
    //通过文本对话框 选中图片路径
    QString filePath = QFileDialog::getOpenFileName(this);
    ui->lineEditSavePath->setText(filePath);

    // 显示图片
    QPixmap pmp(filePath);
    pmp = pmp.scaledToWidth(ui->labHeader->width());
    ui->labHeader->setPixmap(pmp);
}

void registerWin::on_btnAdd_clicked()
{
    // 1.通过图片，结合faceModel模块得到faceId
    FaceModel faceModel;
    cv::Mat img = cv::imread(ui->lineEditSavePath->text().toUtf8().data());
    int faceId = faceModel.faceRegister(img);

    // 把头像保存到一个固定路径下面
//    QString headerFile = QString("./data/%1.jpg").arg(QString(ui->lineEditName->text().toUtf8().toBase64()));
    QString headerFile = QString("../../data/%1.jpg").arg(QString(ui->lineEditName->text().toUtf8().toBase64()));
    cv::imwrite(headerFile.toUtf8().data(), img);

    // 2. 把个人信息存储到数据库employee
    QSqlTableModel model;
    model.setTable("employee"); // 设置表名
    QSqlRecord record = model.record();
    // 设置数据
    record.setValue("name", ui->lineEditName->text());
    record.setValue("sex", ui->rBtnFemale->isChecked()?"男":"女");
    record.setValue("birthday", ui->dateEditBirth->text());
    record.setValue("address", ui->lineEditAddr->text());
    record.setValue("phone", ui->lineEditPhone->text());
    record.setValue("faceId", faceId);
    //头像路径
    record.setValue("headfile", headerFile);
    // 把记录插入到数据库表中
    bool ret = model.insertRecord(0, record);
    //3.提示注册成功
    if(ret)
    {
        QMessageBox::information(this, "注册提示", "注册成功");
        //提交
        model.submitAll();
    }
    else
    {
       QMessageBox::information(this, "注册提示", "注册失败");
    }

}

void registerWin::on_btnOpenCamera_clicked()
{
   if(ui->btnOpenCamera->text() == "打开摄像头")
   {
       // 打开摄像头
       if(cap.open(0))
       {
           ui->btnOpenCamera->setText("关闭摄像头");
           // 启动定时器事件
           timerid = startTimer(100);
       }
   }
   else
   {
       killTimer(timerid);  // 关闭定时器事件
       ui->btnOpenCamera->setText("打开摄像头");
       // 关闭摄像头
       cap.release();
   }
}


void registerWin::on_btnPhotograph_clicked()
{
    // 保存数据
    // 把图像保存到一个固定路径下
//    QString headerFile = QString("./data/%1.jpg").arg(QString(ui->lineEditName->text().toUtf8().toBase64()));
    QString headerFile = QString("../../data/%1.jpg").arg(QString(ui->lineEditName->text().toUtf8().toBase64()));
    ui->lineEditSavePath->setText(headerFile);
    cv::imwrite(headerFile.toUtf8().data(), img);

    killTimer(timerid);  // 关闭定时器事件
    ui->btnOpenCamera->setText("打开摄像头");
    // 关闭摄像头
    cap.release();
}




