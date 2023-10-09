#include "../include/assist/Res.h"
#include "../include/assist/Mat2QImage.h"
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

Res::Res(const std::string& filePath,QObject* parent)
	:root_mt(cv::imread(filePath))
	,QObject(parent)
{
	if (filePath == "")return;
	update();
	fileInfo = QFileInfo(QString::fromStdString(filePath));
}

void Res::reset(const std::string& filePath)
{
	root_mt = cv::imread(filePath);
	update();
	fileInfo = QFileInfo(QString::fromStdString(filePath));
}

Res::Res(const cv::Mat& mat, QObject* parent)
	:root_mt(mat) 
	, QObject(parent)
{
	update();
	if (get()->work_files.isEmpty()) {
		return;
	}
	fileInfo = QFileInfo(get()->work_files[get()->work_currentIndex]);
}

void Res::reset(const cv::Mat& mat)
{
	root_mt = mat;
	update();
	if (get()->work_files.isEmpty()) {
		return;
	}
	fileInfo = QFileInfo(get()->work_files[get()->work_currentIndex]);
}
