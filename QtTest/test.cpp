#include <QApplication>
#include <QWidget>
#include <QToolBox>
#include <QToolButton>
#include <QGridLayout>
#include <QLabel>
#include <QButtonGroup>
#include <opencv2/opencv.hpp>

class Widget:public QWidget {
public:
	Widget(QWidget* parent = nullptr):QWidget(parent) {
		this->resize(300, 300);
		createToolBox();
	}
	~Widget() {}
	void createToolBox() {
		//Create the buttons for the first page, and put all the buttons in a group to manage, and put the group in the QList to manage
		ls.push_back(btnGroup_1 = new QButtonGroup);
		btnGroup_1->setExclusive(true);

		//Connect signals and slots, to achieve click button
		connect(btnGroup_1, &QButtonGroup::buttonClicked, this, &Widget::onTriggered_btnGroup_1);

		//Create a GridLayout and place the two buttons
		QGridLayout* gird_1 = new QGridLayout;
		gird_1->addWidget(createToolButton("toolButton_a1",1), 0, 0);
		gird_1->addWidget(createToolButton("toolButton_a2",1), 1, 0);

		//Finally create a widget and set layout above.
		QWidget* widget_1 = new QWidget;
		widget_1->setLayout(gird_1);

		//----------------------------------
		//Do exactly the same as above, to create the second page content.
		ls.push_back(btnGroup_2 = new QButtonGroup);
		btnGroup_2->setExclusive(true);

		connect(btnGroup_2, &QButtonGroup::buttonClicked, this, &Widget::onTriggered_btnGroup_2);

		QGridLayout* gird_2 = new QGridLayout;
		gird_2->addWidget(createToolButton("toolButton_b1",2), 0, 0);
		gird_2->addWidget(createToolButton("toolButton_b2",2), 1, 0);

		QWidget* widget_2 = new QWidget;
		widget_2->setLayout(gird_2);


		//--------------------------------
		// Create a QToolBox and add the two pages.
		toolBox = new QToolBox(this);
		toolBox->setMinimumWidth(200);
		toolBox->setMaximumWidth(200);
		toolBox->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
		toolBox->addItem(widget_1, "widget_1");
		toolBox->addItem(widget_2, "widget_2");

		//Most important of all. 
		//When I switch pages I need to reset all the buttons on the previous page to unselected
		connect(toolBox, &QToolBox::currentChanged, this, [=](int value) {
			//record the previous page index. I am sure this value is right.
			preToolBoxIndex = curToolBoxIndex;
			curToolBoxIndex = value;

			//to get the previous page's all buttons;
			
			QList<QAbstractButton*> btns = ls[preToolBoxIndex]->buttons();
			ls[preToolBoxIndex]->setExclusive(false);
			for (auto& x : btns) {
				x->setChecked(false);
				//Most confusion is that if the state of x is true, even if setChecked(false) is done, it still prints true in this output.
				qInfo() << x->isChecked();
			}
			ls[preToolBoxIndex]->setExclusive(true);
		});
	}
	//to create toolbutton
	QWidget* createToolButton(const QString& text,int type) {
		QToolButton* btn = new QToolButton;
		btn->setObjectName("function_tbtn");
		btn->resize(48, 48);
		btn->setCheckable(true);
		btn->setChecked(false);

		if (type == 1) {
			btnGroup_1->addButton(btn,type);
		}
		else if (type == 2) {
			btnGroup_2->addButton(btn, type);
		}

		QGridLayout* grid = new QGridLayout;
		grid->addWidget(btn, 0, 0, Qt::AlignHCenter);
		QLabel* textlab = new QLabel(text);
		textlab->setObjectName("textLab");
		grid->addWidget(textlab, 1, 0, Qt::AlignHCenter);

		QWidget* wid = new QWidget;
		wid->setLayout(grid);

		return wid;
	}
public slots:
	void onTriggered_btnGroup_1(QAbstractButton* btn) {
		//choice the clicked button and setChecked(true).
		QList<QAbstractButton*> btns = btnGroup_1->buttons();
		for (auto& m_btn : btns) {
			if (m_btn == btn) {
				m_btn->setChecked(true);
			}
		}
	}
	void onTriggered_btnGroup_2(QAbstractButton* btn) {
		QList<QAbstractButton*> btns = btnGroup_2->buttons();
		for (auto& m_btn : btns) {
			if (m_btn == btn) {
				m_btn->setChecked(true);
			}
		}
	}
private:
	QButtonGroup* btnGroup_1 = nullptr;
	QButtonGroup* btnGroup_2 = nullptr;

	//QList to manager all QButtolGroup
	QList<QButtonGroup*> ls;


	QToolBox* toolBox = nullptr;
	int preToolBoxIndex = 0;
	int curToolBoxIndex = 0;

};

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
		
	cv::Mat m = cv::imread("../resource/dog.png");
	cv::imshow("test", m);
	cv::Mat grayC;
	cv::cvtColor(m, grayC, cv::COLOR_BGR2GRAY);
	cv::Mat BGR;
	cv::cvtColor(grayC, BGR, cv::COLOR_GRAY2BGR);
	cv::Mat hsv;
	//请注意，由于灰度图像只包含亮度信息，转换为BGR格式后，无法恢复原始彩色信息。因此，转换后的图像将呈现灰度的单色效果。
	cv::cvtColor(BGR, hsv, cv::COLOR_BGR2HSV);
	cv::imshow("gray", grayC);
	cv::imshow("bgr", BGR);
	cv::imshow("hsv", hsv);
	cv::waitKey(0);

	return a.exec();
}