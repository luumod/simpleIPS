#include "Include/opencv_functions/Connected.h"
#include "Include/assist/Mat2QImage.h"
#include "Include/Widget/widget.h"
#include <QString>

Connected::Connected() :Object() {}

Connected::~Connected() {}

void Connected::initialize()
{
	int current_choice = -1;
	int connectivity = 8, ccltype = cv::CCL_DEFAULT;
}

void Connected::convert(const cv::Mat& src, cv::Mat& out, int nConAreas)
{
	/*
	src 是经过连通性分析后得到的灰度图，需要进行转换为BGR图然后着色
	*/
	srand(time(nullptr));
	std::vector<cv::Vec3b> colors(nConAreas);
	for (auto& x : colors) {
		x = cv::Vec3b(rand() % 256, rand() % 256, rand() % 256 );
	}
	cv::Mat ansImg = cv::Mat::zeros(src.size(), CV_8UC3);
	for (int i = 0; i < src.rows; i++) {
		for (int j = 0; j < src.cols; j++) {
			int lab = src.at<int>(i, j);
			if (lab >= 1 && lab < nConAreas) { //nConAreas会多一个
				ansImg.at<cv::Vec3b>(i,j) = colors[lab];
			}
			else if (lab == 0){
				ansImg.at<cv::Vec3b>(i,j) = cv::Vec3b(255, 255, 255);
			}
		}
	}
	out = ansImg;
}

void Connected::handle(cv::Mat& tMt,cv::Mat& tMt2) {
	/*
	tMt2为转换后的阈值图
	*/
	cv::Mat _mt;
	getMat(_mt);
	//转换为灰度图
	cv::cvtColor(_mt, tMt, cv::COLOR_BGR2GRAY);
	//阈值化
	cv::threshold(tMt, tMt2, 128, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
}

void Connected::connectedComponents()
{
	cv::Mat tMt,tMt2;
	
	handle(tMt, tMt2);

	//areas 连通块个数
	int areas = cv::connectedComponents(tMt2, tMt, connectivity, CV_32S);
	
	convert(tMt, tMt2, areas);

	Object::update(tMt2);

}

void Connected::connectedComponentsWithStats()
{
	cv::Mat tMt, tMt2;

	handle(tMt, tMt2);

	cv::Mat states,center; //status: cv::ConnectedComponentsTypes
	int nConAreas = cv::connectedComponentsWithStats(tMt2, tMt, states, center, connectivity, CV_32S, ccltype);

	convert(tMt, tMt2, nConAreas);//tMt2

	//用框框把连通区域框起来
	for (int i = 1; i < nConAreas; i++) {
		//states.at<i> 存储了每一个连通块的位置坐标
		int x = states.at<int>(i, cv::ConnectedComponentsTypes::CC_STAT_LEFT);
		int y = states.at<int>(i, cv::ConnectedComponentsTypes::CC_STAT_TOP);
		int w = states.at<int>(i, cv::ConnectedComponentsTypes::CC_STAT_WIDTH);
		int h = states.at<int>(i, cv::ConnectedComponentsTypes::CC_STAT_HEIGHT);

		cv::rectangle(tMt2,
			cv::Rect(x, y, w, h), cv::Scalar(255, 0, 0));
	}

	Object::update(tMt2);

}


void Connected::onTriggered_Comb1_currentTextChanged_connectivtiy(int index)
{
	if (index == 0) {
		connectivity = 8;
	}
	else if (index == 1) {
		connectivity = 4;
	}

	if (current_choice == 0) {
		connectedComponents();
	}
	else if (current_choice == 1) {
		connectedComponentsWithStats();
	}
}


void Connected::onTriggered_Comb2_currentTextChanged_ccltype(int index)
{
	// 连通算法改变
	ccltype = cv::ConnectedComponentsAlgorithmsTypes(index - 1);

	switch (current_choice)
	{
	case CONNECTED::CONNECTED_TYPE1:
		connectedComponents();
		return;
	case CONNECTED::CONNECTED_TYPE2:
		connectedComponentsWithStats();
		return;
	default:
		return;
	}
}
