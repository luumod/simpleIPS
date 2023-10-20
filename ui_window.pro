QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    source/BaseOperate.cpp \
    source/Blur.cpp \
    source/CaptureWidget.cpp \
    source/Connected.cpp \
    source/Contours.cpp \
    source/DrawWidget.cpp \
    source/GraphicsScene.cpp \
    source/LookWidget.cpp \
    source/Morphology.cpp \
    source/Object.cpp \
    source/Res.cpp \
    source/ShowImgWidget.cpp \
    source/Showeffect.cpp \
    source/Threshold.cpp \
    source/WidgetBuilder.cpp \
    source/eventfilterobject.cpp \
    source/main.cpp \
    source/widget.cpp

HEADERS += \
    Widget/CaptureWidget.h \
    Widget/DrawWidget.h \
    Widget/LookWidget.h \
    Widget/ShowImgWidget.h \
    Widget/WidgetBuilder.h \
    Widget/widget.h \
    Widget/widget_include_files.h \
    assist/Common.h \
    assist/Enums.h \
    assist/Mat2QImage.h \
    assist/QImage2Mat.h \
    assist/Res.h \
    assist/belongsToEnum.h \
    assist/config.h \
    opencv_functions/BaseOperate.h \
    opencv_functions/Blur.h \
    opencv_functions/Connected.h \
    opencv_functions/Contours.h \
    opencv_functions/Morphology.h \
    opencv_functions/Object.h \
    opencv_functions/Showeffect.h \
    opencv_functions/Threshold.h \
    other_functions/GraphicsScene.h \
    other_functions/eventfilterobject.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Tools/openCV/openCV/build/x64/vc16/lib/ -lopencv_world470
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Tools/openCV/openCV/build/x64/vc16/lib/ -lopencv_world470d

INCLUDEPATH += $$PWD/../../../Tools/openCV/openCV/build/include
DEPENDPATH += $$PWD/../../../Tools/openCV/openCV/build/include

DESTDIR = F:/code/simplePS_work/simpleIPS

DISTFILES += \
    resource/assert/0.png \
    resource/assert/2.png \
    resource/assert/avg.png \
    resource/assert/begin.png \
    resource/assert/bi.png \
    resource/assert/colors.png \
    resource/assert/convert.png \
    resource/assert/dingmao.png \
    resource/assert/draw.png \
    resource/assert/duibiduzhankuan.png \
    resource/assert/erjinzhi.png \
    resource/assert/f0.png \
    resource/assert/f2.png \
    resource/assert/fushi.png \
    resource/assert/gamma.png \
    resource/assert/gaussian.png \
    resource/assert/heimao.png \
    resource/assert/huiji.png \
    resource/assert/jieduan.png \
    resource/assert/kai.png \
    resource/assert/liantongkuai.png \
    resource/assert/liantongkuais.png \
    resource/assert/light.png \
    resource/assert/linear.png \
    resource/assert/lunkuo.png \
    resource/assert/minus.png \
    resource/assert/non_linear.png \
    resource/assert/pengzhang.png \
    resource/assert/previewOk.png \
    resource/assert/restore.png \
    resource/assert/return.png \
    resource/assert/right_arrow.png \
    resource/assert/shuangbian.png \
    resource/assert/suiji.png \
    resource/assert/tidu.png \
    resource/assert/zhong.png \
    resource/bigImages/2.png \
    resource/bigImages/3.png \
    resource/bigImages/5.png \
    resource/bigImages/6.png \
    resource/bigImages/大图片测试 \
    resource/qss/dark.css \
    resource/qss/light.css \
    resource/testImages/103.png \
    resource/testImages/104.png \
    resource/testImages/105.png \
    resource/testImages/106.png \
    resource/testImages/107.png \
    resource/testImages/113.png \
    resource/testImages/114.png \
    resource/testImages/115.png \
    resource/testImages/116.png \
    resource/testImages/122.png \
    resource/testImages/批量图片测试 \
    resource/如何添加一个新功能.md \
    resource/窗口结构.png


