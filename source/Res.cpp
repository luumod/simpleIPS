#include "Include/assist/Res.h"
#include "Include/assist/Mat2QImage.h"
#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QStandardPaths>

Widget* Res::get()
{
	return Widget::getInstance();
}

void Res::update()
{
	inter_mt = root_mt.clone();
	flash_mt = root_mt.clone();
	curr_mt = root_mt.clone();
	curr_img = Mat2QImage(curr_mt);
}

Res::~Res()
{
}

void Res::updateFileInfo(const QString& filePath) {
	fileInfo = QFileInfo(filePath);
	QString fileName = fileInfo.fileName(); // 获取文件名，这里会得到 "dog.png"
	QString pathWithoutFileName = fileInfo.path(); // 获取文件名之前的路径，这里会得到 "F:/code/simplePS_work/myPhotoshopApp/bin"
}


Res::Res(const std::string& filePath,QObject* parent)
    :QObject(parent)
    ,root_mt(cv::imread(filePath))
{
	if (filePath == "")return;
	update();
	fileInfo = QFileInfo(QString::fromLocal8Bit(filePath.c_str()));
	emit signal_updateImage();
}

void Res::reset(const std::string& filePath)
{
	root_mt = cv::imread(filePath);
	update();
	fileInfo = QFileInfo(QString::fromLocal8Bit(filePath.c_str()));
	emit signal_updateImage();
}

Res::Res(const cv::Mat& mat, QObject* parent)
    : QObject(parent)
    ,root_mt(mat)
{
	update();
	if (get()->work_files.isEmpty()) {
		return;
	}
	fileInfo = QFileInfo(get()->work_files[get()->work_currentIndex]);
	emit signal_updateImage();
}

void Res::reset(const cv::Mat& mat)
{
	root_mt = mat;
	update();
	if (get()->work_files.isEmpty()) {
		return;
	}
	fileInfo = QFileInfo(get()->work_files[get()->work_currentIndex]);
	emit signal_updateImage();
}
