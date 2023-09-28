#ifndef BASEOPERATE_H_
#define BASEOPERATE_H_

/// <summary>
/// 图像基础操作
/// author: Yuleo
/// </summary>
/// 

#include "Object.h"
#include <QWidget>
#include <opencv2/opencv.hpp>
class QImage;

class BaseOperate :public Object {
public:
	BaseOperate();
	~BaseOperate();

	void initialize()override;
	
public slots:
	void cvtColor(QAction* action);
	void onTriggered_picture_rotate90();
	void onTriggered_picture_rotate180();
	void onTriggered_picture_rotate270();
public:
	
};


#endif  //!BASEOPERATE_H_