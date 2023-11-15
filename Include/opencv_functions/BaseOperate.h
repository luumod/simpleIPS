#ifndef BASEOPERATE_H_
#define BASEOPERATE_H_

/*
 图像基础操作
*/

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;
class ShowImgWidget;
class QLabel;

class BaseOperate :public Object {
public:
	BaseOperate();
	~BaseOperate();

protected:
	void initialize()override;
	
public slots:
	void cvtColor(QAction* action,int num);
	//旋转
	void onTriggered_picture_rotate90();
	void onTriggered_picture_rotate180();
	void onTriggered_picture_rotate270();
	//翻转
	void onTriggered_picture_flip(int flag);
	void onTriggered_picture_flip0();
	void onTriggered_picture_flip1();
	void onTriggered_picture_flip_1();

	//对比度提高
	void onTriggered_picture_mask();

	//显示灰度直方图
    void drawGrayHist(const QString& title, cv::Mat mt = cv::Mat());
    QImage showGrayHist_AdjArea(const QString& title,int height,int width);

	//显示均衡化后的图像
	void showEqualizedBGRImage();
	void showEqualizedGrayImage();
private:
	cv::Mat getHist(cv::Mat mt);
	void new_showWidget();
public:
	ShowImgWidget* show_wid = nullptr;
};


#endif  //!BASEOPERATE_H_
