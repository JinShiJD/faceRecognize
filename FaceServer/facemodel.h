#ifndef FACEMODEL_H
#define FACEMODEL_H

#include <QObject>
#include <seeta/FaceEngine.h>
#include <opencv.hpp>
#include <QtSql>

class FaceModel : public QObject
{
    Q_OBJECT
public:
    explicit FaceModel(QObject *parent = nullptr);
    ~FaceModel();

public slots:
    int64_t faceRegister(cv::Mat& faceImg);
    int faceQuery(cv::Mat& faceImg);

signals:
    void sendFaceId(int64_t faceId);

private:
    seeta::FaceEngine *fEngin;


};

#endif // FACEMODEL_H
