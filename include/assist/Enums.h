#ifndef ENUM_H_
#define ENUM_H_
/*
从 1 开始
*/
enum BLUR {
	Average = 0,
	Gaussian,
	Median,
	Bilateral 
};
enum THRESHOLD { 
	Binary = BLUR::Bilateral + 1,
	Binary_inv,
	Trunc,
	Tozero,
	Tozero_inv
};
enum FORM {
	Erode = THRESHOLD::Tozero_inv + 1,
	Dilate,
	Open,
	Close,
	Gradient,
	Tophat,
	Blackhat,
	Hitmiss 
};
enum CONNECTED {
	CONNECTED_TYPE1 = FORM::Hitmiss + 1,
	CONNECTED_TYPE2
};

enum CONTOURS {
	CONTOURS_TYPE1 = CONNECTED::CONNECTED_TYPE2 + 1,
};

enum SHOW {
	LIGHT = CONTOURS::CONTOURS_TYPE1 + 1,
	GAMMA,
	LINEAR,
	GRAYWINDOW,
	DPLINEAR,
	NON_DPLINEAR,
};


/*
其他操作枚举
*/
enum TYPE {
	BGR = 999,
	BGR555,
	BGR565,
	RGB,
	GRAY,
	HSV,
	HLS,
	LAB,
};

enum SHAPE {
	Line = TYPE::LAB + 1,
	Rect,
	Eillipse,
	Path,

};



#endif