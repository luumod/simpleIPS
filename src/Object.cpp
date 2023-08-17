#include "../include/Object.h"
#include "../include/Mat2QImage.h"
#include <QDebug>
#include <QLabel>

Object::Object()
	:ori_mt(), _mt(), _img()
{
}

Object::Object(const std::string& fileName)
	:ori_mt(cv::imread(fileName)), _img(Mat2QImage(ori_mt))
{
	ori_mt.copyTo(_mt);
}

Object::Object(const cv::Mat& mt)
{
	mt.copyTo(ori_mt);
	ori_mt.copyTo(_mt);
	_img = Mat2QImage(_mt);
}

Object::~Object()
{
}

void Object::savePoint()
{
	_mt.copyTo(savePoint_mt);
}

void Object::returnPoint()
{
	savePoint_mt.copyTo(_mt);
}

void Object::update(const cv::Mat& newMt)
{
	newMt.copyTo(ori_mt);
	restore();
}

void Object::restore()
{
	//重做 / 初始化
	ori_mt.copyTo(_mt);  //恢复原始图片
	_img = Mat2QImage(_mt);

	//TODO ... 不同操作参数的初始化
}