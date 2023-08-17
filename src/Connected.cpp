#include "../include/Connected.h"
#include "../include/Mat2QImage.h"
#include <QString>

Connected::Connected() :Object() {}

Connected::Connected(const std::string& fileName)
	:Object(fileName) {}

Connected::Connected(const cv::Mat& mt)
	:Object(mt) {}

Connected::~Connected() {}

void Connected::initialize()
{
	int current_choice = -1;
	int connectivity = 8, ccltype = cv::CCL_DEFAULT;
}

void Connected::restore()
{
	Object::restore();
	initialize();
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
	//转换为灰度图
	cv::cvtColor(_mt, tMt, cv::COLOR_BGR2GRAY);
	//阈值化
	cv::threshold(tMt, tMt2, 128, 255, cv::ThresholdTypes::THRESH_BINARY_INV);
}

QImage Connected::connectedComponents()
{
	cv::Mat tMt,tMt2;
	
	handle(tMt, tMt2);

	//areas 连通块个数
	int areas = cv::connectedComponents(tMt2, tMt, connectivity, CV_32S);
	
	convert(tMt, tMt2, areas);

	_img = Mat2QImage(tMt2);
	return _img;
}

QImage Connected::connectedComponentsWithStats()
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
	_img = Mat2QImage(tMt2);
	return _img;
}


void Connected::onTriggered_Comb1_currentTextChanged_connectivtiy(const QString& value)
{
	int v = value.toInt();
	if (v == 4 || v == 8) {
		connectivity = v;
		if (current_choice == 0) {
			connectedComponents();
		}
		else if (current_choice ==1){
			connectedComponentsWithStats();
		}
	}
}


void Connected::onTriggered_Comb2_currentTextChanged_ccltype(const QString& value)
{
	QString s = value;
	if (s == "default") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_DEFAULT;
	}
	else if (s == "CC_WU") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_WU;
	}
	else if (s == "CCL_GRANA") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_GRANA;
	}
	else if (s == "CCL_BOLELLI") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_BOLELLI;
	}
	else if (s == "CCL_SAUF") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_SAUF;
	}
	else if (s == "CCL_BBDT") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_BBDT;
	}
	else if (s == "CCL_SPAGHETTI") {
		ccltype = cv::ConnectedComponentsAlgorithmsTypes::CCL_SPAGHETTI;
	}

	if(current_choice == 0) {
		connectedComponents();
	}
	else if (current_choice == 1) {
		connectedComponentsWithStats();
	}
}