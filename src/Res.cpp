#include "../include/assist/Res.h"
#include "../include/assist/Mat2QImage.h"
#include <QDebug>
#include <QFileInfo>

void Res::init()
{
	inter_mt = root_mt.clone();
	preview_mt = root_mt.clone();
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
	init();
	fileInfo = QFileInfo(QString::fromStdString(filePath));
}

void Res::reset(const std::string& filePath)
{
	root_mt = cv::imread(filePath);
	init();
	fileInfo = QFileInfo(QString::fromStdString(filePath));
}

Res::Res(cv::Mat& mat, QObject* parent)
	:root_mt(mat) 
	, QObject(parent)
{
	init();
	fileInfo = QFileInfo(Widget::getInstance()->work_files[Widget::getInstance()->work_currentIndex]);
}

void Res::reset(cv::Mat& mat)
{
	root_mt = mat;
	init();
	fileInfo = QFileInfo(Widget::getInstance()->work_files[Widget::getInstance()->work_currentIndex]);
}
