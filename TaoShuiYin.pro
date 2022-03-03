#-------------------------------------------------
#
# Project created by QtCreator 2016-10-13T09:04:23
#
#-------------------------------------------------

QT       += core gui network svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = TaoShuiYin
TEMPLATE = app


SOURCES += main.cpp\
        taomain.cpp \
    taoconvertcv2qt.cpp \
    taocanvasarea.cpp \
    taopropagateedge.cpp \
    taopushpullapprox.cpp \
    taoimageviewer.cpp \
    taoprocessing.cpp \
    taosettings.cpp \
    taodefinitions.cpp \
    taotextureautocompletion.cpp \
    taotexturemanualcompletion.cpp \
    taohelptemplate.cpp \
    xinpaint/xinpaint.cpp \
    taodownloadfile.cpp \
    taoregister.cpp \
    taowxpay.cpp \
    qrencode/BitBuffer.cpp \
    qrencode/QrCode.cpp \
    qrencode/QrSegment.cpp \
    taocheckexe.cpp \
    taochecknetwork.cpp

HEADERS  += taomain.h \
    taoconvertcv2qt.h \
    taocanvasarea.h \
    taopropagateedge.h \
    taopushpullapprox.h \
    taoimageviewer.h \
    taoprocessing.h \
    taosettings.h \
    taodefinitions.h \
    taotextureautocompletion.h \
    taotexturemanualcompletion.h \
    taohelptemplate.h \
    xinpaint/annf.hpp \
    xinpaint/gcgraph.hpp \
    xinpaint/norm2.hpp \
    xinpaint/photomontage.hpp \
    xinpaint/whs.hpp \
    xinpaint/xinpaint.h \
    taodownloadfile.h \
    taoregister.h \
    taowxpay.h \
    qrencode/BitBuffer.hpp \
    qrencode/QrCode.hpp \
    qrencode/QrSegment.hpp \
    taocheckexe.h \
    taochecknetwork.h

FORMS    += taomain.ui \
    taoimageviewer.ui \
    taoprocessing.ui \
    taohelptemplate.ui \
    taowxpay.ui

INCLUDEPATH += C:\opencv31\include\opencv\
        C:\opencv31\include\opencv2\
        C:\opencv31\include

LIBS    +=C:\opencv31\lib\libopencv_core310.dll.a\
        C:\opencv31\lib\libopencv_imgcodecs310.dll.a\
        C:\opencv31\lib\libopencv_flann310.dll.a\
        C:\opencv31\lib\libopencv_imgproc310.dll.a\
        C:\opencv31\lib\libopencv_photo310.dll.a\
#        C:\opencv31\lib\libopencv_xphoto310.dll.a\
#        C:\opencv31\lib\libopencv_highgui310.dll.a

#        C:\opencv31\lib\libopencv_video310.dll.a\
#        C:\opencv31\lib\libopencv_stitching310.dll.a\

#        C:\opencv31\lib\libopencv_world310.dll.a\

#        C:\opencv31\lib\libopencv_ts310.a\
#        C:\opencv31\lib\libopencv_features2d310.dll.a\
#        C:\opencv31\lib\libopencv_highgui310.dll.a\
#        C:\opencv31\lib\libopencv_objdetect310.dll.a\
#        C:\opencv31\lib\libopencv_video310.dll.a\
#        C:\opencv31\lib\libopencv_calib3d310.dll.a\
#        C:\opencv31\lib\libopencv_ml310.dll.a\

RESOURCES +=

DISTFILES +=

RC_FILE +=

RC_ICONS = Eraser_32px.ico
