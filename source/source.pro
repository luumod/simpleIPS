QT += core gui widgets
SOURCES += \
    BaseOperate.cpp \
    Blur.cpp \
    CaptureWidget.cpp \
    Connected.cpp \
    Contours.cpp \
    DrawWidget.cpp \
    GraphicsScene.cpp \
    LookWidget.cpp \
    Morphology.cpp \
    Object.cpp \
    Res.cpp \
    ShowImgWidget.cpp \
    Showeffect.cpp \
    Threshold.cpp \
    WidgetBuilder.cpp \
    eventfilterobject.cpp \
    main.cpp \
    widget.cpp

LIBS += -L$$PWD/'../../../../vld/Visual Leak Detector/lib/Win64/' -lvld
INCLUDEPATH += $$PWD/'../../../../vld/Visual Leak Detector/include'
DEPENDPATH += $$PWD/'../../../../vld/Visual Leak Detector/include'
