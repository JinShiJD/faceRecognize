#include "facewin.h"
#include "ui_facewin.h"
#include <QDebug>
#include <QThread>
#include <QSqlQuery>
#include <QDateTime>


faceWin::faceWin(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::faceWin)
{
    ui->setupUi(this);

    //qtcpServer 当有客户端发起连接会发送newConnection
    connect(&mserver, &QTcpServer::newConnection, this, &faceWin::accept_client);
    bool ok = mserver.listen(QHostAddress::Any, 9999);
    qDebug() << "listen result: " << ok << mserver.errorString();
    bSize = 0;

    //给sql模型绑定表格
    tModel.setTable("employee");

    // 创建一个线程
    QThread *thread = new QThread();
    // 把faceModel对象移动到thread线程中执行
    fModel.moveToThread(thread);
    // 启动线程
    thread->start();

    connect(this, &faceWin::query, &fModel,&FaceModel::faceQuery);

    //关联faceModel对象中的sendFaceId信号
    connect(&fModel, &FaceModel::sendFaceId, this, &faceWin::recvFaceId);
}

faceWin::~faceWin()
{
    delete ui;
}

// 接收客户端连接
void faceWin::accept_client()
{
    // 获取与客户端通讯的套接字
    msocket = mserver.nextPendingConnection();

    qDebug() << "Client connected from" << msocket->peerAddress().toString();

    // 当客户端有数据到达会发送readyRead信号
    connect(msocket, &QTcpSocket::readyRead, this, &faceWin::readData);

}

//读取客户端发送数据
void faceWin::readData()
{
//    // 读取所有数据
//    QString msg = msocket->readAll();
//    qDebug() << msg;


    //把套接字绑定到数据流
    QDataStream stream(msocket);    // 把套接字绑定到数据表
    stream.setVersion(QDataStream::Qt_5_14);

    if(bSize == 0)
    {
        if(msocket->bytesAvailable() < (qint64)sizeof(bSize)) return;
        // 采集数据的长度
        stream >> bSize;
    }

    if(msocket->bytesAvailable() < bSize)   //数据没发完，返回继续等待
    {
        return;
    }

    QByteArray data;
    stream >> data;
    bSize = 0;
    if(data.size() == 0) // 没有读取到数据
    {
        return;
    }

    //显示图像
    QPixmap pmp;
    pmp.loadFromData(data, "jpg");
    pmp = pmp.scaled(ui->labHeader->size());
    ui->labHeader->setPixmap(pmp);


    // 识别人脸
    cv::Mat faceImg;
    std::vector<uchar> decode;
    decode.resize(data.size());
    memcpy(decode.data(), data.data(), data.size());
    faceImg = cv::imdecode(decode, cv::IMREAD_COLOR);
    //int faceId = fModel.faceQuery(faceImg); // 消耗资源较多

    emit query(faceImg);
}

void faceWin::recvFaceId(uint64_t faceId)
{
    //qDebug() << "FaceId: " << faceId;

    // 从数据库查询faceid对应的个人信息
    // 给模型设置过滤器
    if(faceId < 0)
    {
        QString sdmsg = QString("{\"employeeId\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\",\"headfile\":\"\"}");
        msocket->write(sdmsg.toUtf8()); // 把打包好的数据发送给客户端
        return ;
    }
    tModel.setFilter(QString("faceId=%1").arg(faceId));
    // 查询
    tModel.select();
    // 判断是否查询到数据
    if(tModel.rowCount() == 1)
    {
        // 工号、姓名、部门、时间
        // {employeeId:%1, name:%2, department:软件,time:%3}
        QSqlRecord record = tModel.record(0);
        QString sdmsg = QString("{\"employeeId\":\"%1\",\"name\":\"%2\",\"department\":\"软件\",\"time\":\"%3\",\"headfile\":\"%4\"}")
                .arg(record.value("employeeId").toString()).arg(record.value("name").toString())
                .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"))
                .arg(record.value("headfile").toString());

        // msocket->write(sdmsg.toUtf8()); // 把打包好的数据发送给客户端

        // 把数据写入数据库--考核表
        QString insertSql = QString("insert into attendance(employeeId) values('%1')").arg(record.value("employeeId").toString());
        QSqlQuery query;
        if(!query.exec(insertSql))
        {
            QString sdmsg = QString("{\"employeeId\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\",\"headfile\":\"\"}");
            msocket->write(sdmsg.toUtf8()); // 把打包好的数据发送给客户端
            return ;
        }
        else
        {
            msocket->write(sdmsg.toUtf8()); // 把打包好的数据发送给客户端
        }
    }
}


//void faceWin::recvFaceId(uint64_t faceId)
//{
//    QString sdmsg;
//        if (faceId < 0) {
//            sdmsg = "{\"employeeId\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}";
//            msocket->write(sdmsg.toUtf8());
//            return;
//        }

//        tModel.setFilter(QString("faceId=%1").arg(faceId));
//        tModel.select();
//        if (tModel.rowCount() == 1) {
//            QSqlRecord record = tModel.record(0);
//            QString insertSql = QString("insert into attendance(employeeId) values(%1)")
//                                .arg(record.value("employeeId").toString());
//            QSqlQuery query;
//            if (!query.exec(insertSql)) {
//                qDebug() << "Insert failed:" << query.lastError().text();
//                sdmsg = "{\"employeeId\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}";
//            } else {
//                sdmsg = QString("{\"employeeId\":\"%1\",\"name\":\"%2\",\"department\":\"软件\",\"time\":\"%3\"}")
//                        .arg(record.value("employeeId").toString())
//                        .arg(record.value("name").toString())
//                        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
//            }
//        } else {
//            sdmsg = "{\"employeeId\":\"\",\"name\":\"\",\"department\":\"\",\"time\":\"\"}";
//        }
//        msocket->write(sdmsg.toUtf8());

//}
