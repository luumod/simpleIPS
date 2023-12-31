﻿#include "widget.h"
#include "../assist/Common.h"
#include "../assist/Res.h"
#include "../assist/belongsToEnum.h"
#include "../assist/Mat2QImage.h"

#include "../opencv_functions/Blur.h"
#include "../opencv_functions/Threshold.h"
#include "../opencv_functions/Morphology.h"
#include "../opencv_functions/Connected.h"
#include "../opencv_functions/Contours.h"
#include "../opencv_functions/BaseOperate.h"
#include "../opencv_functions/Showeffect.h"
#include "../other_functions/eventfilterobject.h"

#include "../video/framefilter.h"
#include "../video/frameoperator.h"
#include "../video/videocapwork.h"
#include "ShowImgWidget.h"
#include "CaptureWidget.h"

#include "LookWidget.h"
#include "DrawWidget.h"
#include <QDebug>
#include <QBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QTimer>
#include <opencv2/core/utils/logger.hpp>
#include <QToolButton>
#include <QButtonGroup>
#include <QElapsedTimer>
#include <QToolBox>
#include <QAction>
#include <QMenuBar>
#include <QMenu>

#include <QFileDialog>
#include <QMessageBox>
#include <QScreen>
#include <QApplication>
#include <QThread>
#include <QProgressBar>
#include <QSlider>
#include <QGroupBox>
#include <QFormLayout>
#include <QPainter>
#include <QStackedWidget>
#include <QSizePolicy>
#include <QToolBar>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QPalette>
#include <QColorDialog>
#include <QFile>
#include <QStatusBar>
#include <QActionGroup>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QDialog>
#include <QMouseEvent>
#include <QScreen>
#include <QDockWidget>
#include <QScrollArea>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonValue>
#include <QDesktopServices>
#include <QCheckBox>
#include <QScrollBar>
#include <QFormLayout>
#include <QShortcut>
#include <Windows.h>
#include <QStandardPaths>
#include <QRadioButton>
#include <QTimer>
