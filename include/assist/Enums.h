#ifndef ENUM_H_
#define ENUM_H_
/*
从 1 开始
*/
enum BLUR {
	Average = 1,
	Gaussian,
	Median,
	Bilateral 
};
enum THRESHOLD { 
	Binary = 5,
	Binary_inv,
	Trunc,
	Tozero,
	Tozero_inv
};
enum FORM {
	Erode = 10,
	Dilate,
	Open,
	Close,
	Gradient,
	Tophat,
	Blackhat,
	Hitmiss 
};
enum CONNECTED {
	CONNECTED_TYPE1 = 18,
	CONNECTED_TYPE2
};

enum CONTOURS {
	CONTOURS_TYPE1 = 20,
};

enum CVTCOLOR {
	CVTCOLOR_TYPE1 = 21,
};

/*
显示效果增强
*/
enum SHOW {
	LIGHT = 22,
	SHARPEN,
};

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