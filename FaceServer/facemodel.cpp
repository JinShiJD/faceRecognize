#include "facemodel.h"



FaceModel::FaceModel(QObject *parent) : QObject(parent)
{
    // 初始化
    seeta::ModelSetting FDmode("D:/OpenCV/4.5.2/seetaFace2_install/bin/model/fd_2_00.dat",seeta::ModelSetting::GPU, 0);
    seeta::ModelSetting PDmode("D:/OpenCV/4.5.2/seetaFace2_install/bin/model/pd_2_00_pts5.dat",seeta::ModelSetting::GPU, 0);
    seeta::ModelSetting FRmode("D:/OpenCV/4.5.2/seetaFace2_install/bin/model/fr_2_10.dat",seeta::ModelSetting::GPU, 0);
    this->fEngin = new seeta::FaceEngine(FDmode, PDmode, FRmode);

    // 导入已有的人脸数据库
    this->fEngin->Load("./face.db");
}

FaceModel::~FaceModel()
{
    delete fEngin;
}

int64_t FaceModel::faceRegister(cv::Mat &faceImg)
{
    // 把opencv的Mat数据转化为seetaface的数据
    SeetaImageData sImg;
    sImg.data = faceImg.data;
    sImg.width = faceImg.cols;
    sImg.height = faceImg.rows;
    sImg.channels = faceImg.channels();
    int64_t faceId = this->fEngin->Register(sImg);  //注册返回一个人脸ID
    if(faceId >= 0)
    {
        fEngin->Save("./face.db");
    }
    return faceId;
}

int FaceModel::faceQuery(cv::Mat &faceImg)
{
    // 把opencv的Mat数据转为seetaface的数据
    SeetaImageData sImg;
    sImg.data = faceImg.data;
    sImg.width = faceImg.cols;
    sImg.height = faceImg.rows;
    sImg.channels = faceImg.channels();
    float similarity;
    int64_t faceId = fEngin->Query(sImg, &similarity);   // 运行时间较长
    if(similarity > 0.75)   //相似度
    {
       emit sendFaceId(faceId);
    }
    else
    {
        emit sendFaceId(-1);
    }
    return faceId;

}
