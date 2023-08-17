#ifndef OBJECT_H_
#define OBJECT_H_

#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;

class Object{
public:
	explicit Object();
	explicit Object(const std::string& fileName);
	explicit Object(const cv::Mat& mt);
	virtual ~Object();

public:
	void savePoint();
	void returnPoint();
	void update(const cv::Mat& newMt);
	virtual void initialize() = 0;
	virtual void restore() = 0;

	cv::Mat ori_mt;
	cv::Mat	_mt;
	cv::Mat savePoint_mt;
	QImage _img;
	bool mode = false; //创作 or 测试
};


#endif