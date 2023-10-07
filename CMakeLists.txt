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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# 设置OpenCV安装路径
set(OpenCV_DIR "F:\\Tools\\openCV\\openCV\\build")

# 查找并引用OpenCV库
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

# 添加可执行文件或库
add_executable(QtPro
    src/widget.cpp
    src/Blur.cpp
    src/Threshold.cpp
    src/Morphology.cpp
    src/main.cpp
    src/Connected.cpp
    src/Object.cpp
    src/LabelImg.cpp
    src/Contours.cpp
    src/Res.cpp

    src/DrawWidget.cpp
    src/GraphicsScene.cpp
    src/BaseOperate.cpp
    src/Showeffect.cpp
    src/eventfilterobject.cpp
    src/ShowImgWidget.cpp
    src/CaptureWidget.cpp

    include/assist/Common.h
    include/assist/belongsToEnum.h 
    include/assist/Mat2QImage.h
    include/assist/Enums.h 
    include/assist/Res.h 

    include/opencv_functions/Blur.h
    include/opencv_functions/Threshold.h 
    include/opencv_functions/Morphology.h
    include/opencv_functions/Connected.h
    include/opencv_functions/Object.h 
    include/opencv_functions/Contours.h
    include/opencv_functions/BaseOperate.h
    include/opencv_functions/Showeffect.h

    include/other_functions/GraphicsScene.h
    include/other_functions/eventfilterobject.h

    include/Widget/LabelImg.h 
    include/Widget/DrawWidget.h 
    include/Widget/ShowImgWidget.h
    include/Widget/CaptureWidget.h

    include/widget_include_files.h
    include/widget.h
 "src/NativeEventFilter.cpp" "include/assist/config.h"  "include/assist/QImage2Mat.h")
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