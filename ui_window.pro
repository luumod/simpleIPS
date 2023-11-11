QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

FORMS += \
    mainwindow.ui


qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# opencv配置
win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../Tools/openCV/openCV/build/x64/vc16/lib/ -lopencv_world470
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../Tools/openCV/openCV/build/x64/vc16/lib/ -lopencv_world470d
INCLUDEPATH += $$PWD/../../../Tools/openCV/openCV/build/include
DEPENDPATH += $$PWD/../../../Tools/openCV/openCV/build/include

# vld 配置
LIBS += -L$$PWD/'../../../vld/Visual Leak Detector/lib/Win64/' -lvld
INCLUDEPATH += $$PWD/'../../../vld/Visual Leak Detector/include'
DEPENDPATH += $$PWD/'../../../vld/Visual Leak Detector/include'

# 目标程序生成位置
DESTDIR = $$PWD/bin
LIBS += $$PWD/bin/colordialogplugin.lib


DISTFILES += \
    resource/Images/103.png \
    resource/Images/104.png \
    resource/Images/105.png \
    resource/Images/106.png \
    resource/Images/107.png \
    resource/Images/113.png \
    resource/Images/114.png \
    resource/Images/115.png \
    resource/Images/116.png \
    resource/Images/122.png \
    resource/Images/批量图片测试 \
    resource/bigImages/2.png \
    resource/bigImages/3.png \
    resource/bigImages/5.png \
    resource/bigImages/6.png \
    resource/bigImages/night.jpg \
    resource/bigImages/大图片测试 \
    resource/assert/0.png \
    resource/assert/2.png \
    resource/assert/about.png \
    resource/assert/aboutme.png \
    resource/assert/avg.png \
    resource/assert/begin.png \
    resource/assert/bi.png \
    resource/assert/capture.png \
    resource/assert/colors.png \
    resource/assert/convert.png \
    resource/assert/dingmao.png \
    resource/assert/disp.png \
    resource/assert/draw.png \
    resource/assert/duibiduzhankuan.png \
    resource/assert/erjinzhi.png \
    resource/assert/exit.png \
    resource/assert/f0.png \
    resource/assert/f2.png \
    resource/assert/fushi.png \
    resource/assert/gamma.png \
    resource/assert/gaussian.png \
    resource/assert/heimao.png \
    resource/assert/hide.png \
    resource/assert/histogram.png \
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
    resource/assert/open.png \
    resource/assert/open_works.png \
    resource/assert/pengzhang.png \
    resource/assert/previewOk.png \
    resource/assert/restore.png \
    resource/assert/return.png \
    resource/assert/right_arrow.png \
    resource/assert/save.png \
    resource/assert/shuangbian.png \
    resource/assert/suiji.png \
    resource/assert/tidu.png \
    resource/assert/zhong.png \
    resource/qss/dark.css \
    resource/qss/light.css

HEADERS += \
    Include/Widget/CaptureWidget.h \
    Include/Widget/DrawWidget.h \
    Include/Widget/LookWidget.h \
    Include/Widget/ShowImgWidget.h \
    Include/Widget/WidgetBuilder.h \
    Include/Widget/widget.h \
    Include/Widget/widget_include_files.h \
    Include/assist/Common.h \
    Include/assist/Enums.h \
    Include/assist/Mat2QImage.h \
    Include/assist/QImage2Mat.h \
    Include/assist/Res.h \
    Include/assist/belongsToEnum.h \
    Include/assist/config.h \
    Include/opencv_functions/BaseOperate.h \
    Include/opencv_functions/Blur.h \
    Include/opencv_functions/Connected.h \
    Include/opencv_functions/Contours.h \
    Include/opencv_functions/Morphology.h \
    Include/opencv_functions/Object.h \
    Include/opencv_functions/Showeffect.h \
    Include/opencv_functions/Threshold.h \
    Include/other_functions/GraphicsScene.h \
    Include/other_functions/deprecated.h \
    Include/other_functions/eventfilterobject.h \
    colorDialog.h

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

RESOURCES += \
    icons.qrc
