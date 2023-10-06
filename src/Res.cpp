#include "../include/Res.h"
#include "../include/assist/Mat2QImage.h"
#include <QDebug>

void Res::init()
{
	inter_mt = root_mt.clone();
	preview_mt = root_mt.clone();
	curr_mt = root_mt.clone();
	curr_img = Mat2QImage(curr_mt);
}

Res::Res(const std::string& filePath,QObject* parent)
	:root_mt(cv::imread(filePath))
	,QObject(parent)
{
	init();
}


Res::Res(cv::Mat& mat, QObject* parent)
	:root_mt(mat) 
	, QObject(parent)
{
	init();
}

Res::~Res()
{
}

void Res::reset(const std::string& filePath)
{
	root_mt = cv::imread(filePath);
	init();
}

void Res::reset(cv::Mat& mat)
{
	root_mt = mat;
	init();
}
