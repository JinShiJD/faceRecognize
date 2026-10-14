QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


#windows 平台opencv, seetaface环境
win32{
#opecv和seetaface的库文件
LIBS +=D:/OpenCV/4.5.2/opencv_install/x64/mingw/lib/libopencv*
LIBS +=D:/OpenCV/4.5.2/seetaFace2_install/lib/libSeeta*


INCLUDEPATH += D:\OpenCV\4.5.2\opencv_install\include
INCLUDEPATH += D:\OpenCV\4.5.2\opencv_install\include\opencv2
INCLUDEPATH += D:\OpenCV\4.5.2\seetaFace2_install\include
INCLUDEPATH += D:\OpenCV\4.5.2\seetaFace2_install\include\seeta
}

#unix{
#LIBS += -L/opt/opencv-pt/lib -lopencv_world \
#-lSeetaFaceDetector \
#-lSeetaFaceLandmarker \
#-lSeetaFaceRecognizeer \
#-lSeetaNet \
#-lSeetaQualityAssessor \

#INCLUDEPATH += /opt/opencv4-pc/include/

#INCLUDEPATH += /opt/opencv4-pc/opencv_install/include
#INCLUDEPATH += /opt/opencv4-pc/opencv_install/include/opencv2
#INCLUDEPATH += /opt/opencv4-pc/seetaFace2_install/include
#INCLUDEPATH += /opt/opencv4-pc/seetaFace2_install/include/seeta
#}



SOURCES += \
    main.cpp \
    faceattandance.cpp

HEADERS += \
    faceattandance.h

FORMS += \
    faceattandance.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc
