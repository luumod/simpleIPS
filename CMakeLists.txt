cmake_minimum_required(VERSION 3.8)

project(QtPro)
project(QtTest)


# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

# 查找并引用Qt6库
find_package(Qt6 COMPONENTS Core Widgets REQUIRED)

# 设置调试
set(CMAKE_BUILD_TYPE Debug)

# 指定目标exe生成位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/core)

# 设置OpenCV安装路径
set(OpenCV_DIR "F:\\Tools\\openCV\\openCV\\build")

# 查找并引用OpenCV库
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

# 添加可执行文件或库
add_executable(QtPro
    core/src/widget.cpp
    core/src/Blur.cpp
    core/src/Threshold.cpp
    core/src/Morphology.cpp
    core/src/main.cpp
    core/src/Connected.cpp
    core/src/Object.cpp
    core/src/LookWidget.cpp
    core/src/Contours.cpp
    core/src/Res.cpp

    core/src/DrawWidget.cpp
    core/src/GraphicsScene.cpp
    core/src/BaseOperate.cpp
    core/src/Showeffect.cpp
    core/src/eventfilterobject.cpp
    core/src/ShowImgWidget.cpp
    core/src/CaptureWidget.cpp
    core/src/NativeEventFilter.cpp
    core/src/WidgetBuilder.cpp


    core/include/assist/Common.h
    core/include/assist/belongsToEnum.h 
    core/include/assist/Mat2QImage.h
    core/include/assist/Enums.h 
    core/include/assist/Res.h 
    core/include/assist/config.h
    core/include/assist/QImage2Mat.h

    core/include/opencv_functions/Blur.h
    core/include/opencv_functions/Threshold.h 
    core/include/opencv_functions/Morphology.h
    core/include/opencv_functions/Connected.h
    core/include/opencv_functions/Object.h 
    core/include/opencv_functions/Contours.h
    core/include/opencv_functions/BaseOperate.h
    core/include/opencv_functions/Showeffect.h

    core/include/other_functions/GraphicsScene.h
    core/include/other_functions/eventfilterobject.h

    core/include/Widget/LookWidget.h
    core/include/Widget/DrawWidget.h 
    core/include/Widget/ShowImgWidget.h
    core/include/Widget/CaptureWidget.h

    core/include/widget_include_files.h
    core/include/widget.h
    core/include/Widget/WidgetBuilder.h 
)
 add_executable(QtTest
    QtTest/test1.cpp 
 )



# 将Qt6和OpenCV库链接到目标可执行文件或库
target_link_libraries(QtPro
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    ${OpenCV_LIBS}
)
target_link_libraries(QtTest
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    ${OpenCV_LIBS}
)

    

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_definitions(-DUNICODE -D_UNICODE)


# 以下内容可以直接删除
set(VLD_INCLUDE_DIRS "F:\\vld\\Visual Leak Detector\\include")
set(VLD_LIBRARIES "F:\\vld\\Visual Leak Detector\\lib\\Win64\\vld.lib")
include_directories(${VLD_INCLUDE_DIRS})
target_link_libraries(QtPro PRIVATE  ${VLD_LIBRARIES})
target_link_libraries(QtTest PRIVATE  ${VLD_LIBRARIES})