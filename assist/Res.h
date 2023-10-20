#ifndef RES_H_
#define RES_H_

#include "Widget/widget.h"
#include <opencv2/opencv.hpp>
#include <string>
#include <QObject>
#include <QImage>
#include <QFileInfo>
/*
加载图片资源
*/

class Res:public QObject {
	Q_OBJECT
public:
	static Widget* get();
	Res(const std::string& filePath="", QObject* parent = nullptr);
	Res(const cv::Mat& mat, QObject* parent = nullptr);
	~Res();
	void reset(const std::string& filePath);
	void reset(const cv::Mat& mat);

	//资源更新
	void update();
	void updateFileInfo(const QString& filePath);
signals:
	void signal_updateImage();
public:
	cv::Mat	root_mt; //root根层：保存原始的加载的图片
	cv::Mat inter_mt; //intermediate中间层：普通模式下对此图片进行操作
	cv::Mat flash_mt; //flash预览层：加工模式下对此图片进行预操作
	cv::Mat curr_mt; //current当前层：lab_img的实际显示图片
	QImage  curr_img;
	
	//属性信息
	QFileInfo fileInfo;
}; 

#endif // !RES_H_
