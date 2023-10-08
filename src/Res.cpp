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

Res::Res(QObject* parent)
	:QObject(parent) {
	QString fileName = QFileDialog::getOpenFileName(nullptr, "选择文件", ".", "图像文件(*.png *.jpg)");
	if (!fileName.isEmpty()) {
		root_mt = cv::imread(fileName.toLocal8Bit().data());
		init();
		fileInfo = QFileInfo(fileName);
	}
	else {
		exit(0);
	}
}

Res::Res(const std::string& filePath,QObject* parent)
	:root_mt(cv::imread(filePath))
	,QObject(parent)
{
	if (filePath == "")return;
	init();
	fileInfo = QFileInfo(QString::fromStdString(filePath));
}

void Res::reset(const std::string& filePath)
{
	root_mt = cv::imread(filePath);
	init();
	fileInfo = QFileInfo(QString::fromStdString(filePath));
}

Res::Res(const cv::Mat& mat, QObject* parent)
	:root_mt(mat) 
	, QObject(parent)
{
	init();
	if (get()->work_files.isEmpty()) {
		return;
	}
	fileInfo = QFileInfo(get()->work_files[get()->work_currentIndex]);
}

void Res::reset(const cv::Mat& mat)
{
	root_mt = mat;
	init();
	if (get()->work_files.isEmpty()) {
		return;
	}
	fileInfo = QFileInfo(get()->work_files[get()->work_currentIndex]);
}
