#include "facewin.h"
#include <QApplication>

#include <QtSql>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <opencv.hpp>

#include "registerwin.h"
#include "querywin.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<cv::Mat>("cv::Mat&");
    qRegisterMetaType<cv::Mat>("cv::Mat");
    qRegisterMetaType<int64_t>("int64_t");


//    registerWin regW;
//    regW.show();

    //连接数据库
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //设置数据名称
    db.setDatabaseName("server.db");
    //打开数据库
    if(!db.open())
    {
      qDebug() << db.lastError().text();
      return -1;
    }

    // 创建员工信息表                                                                                                              //没写完
    QString createSql = "create table if not exists employee(employeeId integer primary key autoincrement, name varchar(256), sex varchar(32),"
            "birthday text, address text, phone text, faceId integer unique, headfile text)";

    QSqlQuery query;
    if(!query.exec(createSql))
    {
        qDebug() << query.lastError().text();
        return -1;
    }

    // 考勤表
    createSql = "create table if not exists attendance(attendanceId integer primary key autoincrement, employeeId integer,"
            "attendanceTime TimeStamp NOT NULL DEFAULT(datetime('now', 'localtime')))";

    if(!query.exec(createSql))
    {
        qDebug() << query.lastError().text();
        return -1;
    }

    faceWin w;
    w.show();

//    queryWin qW;
//    qW.show();

    return a.exec();
}
