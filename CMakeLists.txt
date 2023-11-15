cmake_minimum_required(VERSION 3.8)

project(QtPro)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(UI ${CMAKE_CURRENT_LIST_DIR}/mainwindow.ui)

set(PROJECT_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
include_directories(QtPro PRIVATE ${PROJECT_ROOT_DIR})

# 查找并引用Qt6库
find_package(Qt6 COMPONENTS Core Widgets OpenGLWidgets REQUIRED)

# 指定目标exe生成位置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/bin)

# 设置OpenCV安装路径
set(OpenCV_DIR "F:\\Tools\\openCV\\openCV\\build")

# 查找并引用OpenCV库
find_package(OpenCV REQUIRED PATHS ${OpenCV_DIR} NO_DEFAULT_PATH)

FILE(GLOB SOURCE_FILES "source/*.cpp") 
FILE(GLOB HEADER_FILES "Include/assist/*.h" "Include/opencv_functions/*.h" "Include/other_functions/*.h" "Include/video/*.h" "Include/Widget/*.h") 

#SOURCE_GROUP("UI" FILES ${UI_FILES} ${WRAP_FILES})
set(RCC_FILES icons.qrc)
qt_wrap_ui(UI_HEADERS ${UI})

#  ${WRAP_FILES}
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${RCC_FILES} ${UI_HEADERS})

# 将Qt6和OpenCV库链接到目标可执行文件或库
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::OpenGLWidgets
    opengl32
    glu32
    ${OpenCV_LIBS}
)

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_definitions(-DUNICODE -D_UNICODE)


# 以下内容可以直接删除
set(VLD_INCLUDE_DIRS "F:\\vld\\Visual Leak Detector\\include")
set(VLD_LIBRARIES "F:\\vld\\Visual Leak Detector\\lib\\Win64\\vld.lib")
include_directories(${VLD_INCLUDE_DIRS})
target_link_libraries(QtPro PRIVATE  ${VLD_LIBRARIES})