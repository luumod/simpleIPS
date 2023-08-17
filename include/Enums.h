#ifndef ENUM_H_
#define ENUM_H_

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
	TYPE1 = 18,
	TYPE2
};

#endif