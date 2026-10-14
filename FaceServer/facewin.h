#ifndef FACEWIN_H
#define FACEWIN_H

#include "facemodel.h"

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>

#include <QSqlTableModel>
#include <QSqlRecord>

QT_BEGIN_NAMESPACE
namespace Ui { class faceWin; }
QT_END_NAMESPACE

class faceWin : public QMainWindow
{
    Q_OBJECT

public:
    faceWin(QWidget *parent = nullptr);
    ~faceWin();

signals:
    void query(cv::Mat& img);

private slots:
    void accept_client();
    void readData();
    void recvFaceId(uint64_t faceId);

private:
    Ui::faceWin *ui;

    QTcpServer mserver;
    QTcpSocket *msocket;
    quint64 bSize;

    FaceModel fModel;
    QSqlTableModel tModel;
};
#endif // FACEWIN_H
