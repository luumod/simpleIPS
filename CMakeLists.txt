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

    include/widget.h
    include/belongsToEnum.h 
    include/Blur.h
    include/Mat2QImage.h
    include/Enums.h 
    include/Threshold.h 
    include/Morphology.h
    include/Connected.h  
    include/Object.h  
    include/QImage2Mat.h)

add_executable(QtTest
    QtTest/test.cpp  )



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

set(VLD_INCLUDE_DIRS "F:\\vld\\Visual Leak Detector\\include")
set(VLD_LIBRARIES "F:\\vld\\Visual Leak Detector\\lib\\Win64\\vld.lib")
include_directories(${VLD_INCLUDE_DIRS})
target_link_libraries(QtPro PRIVATE  ${VLD_LIBRARIES})
target_link_libraries(QtTest PRIVATE  ${VLD_LIBRARIES})