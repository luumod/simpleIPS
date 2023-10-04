#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtGui/QPixmap>
#include <QtCore/QDir>
#include <QtCore/QFileInfoList>
#include <QVBoxLayout>
#include <QStackedWidget>

class ImageViewer : public QMainWindow {
public:
	ImageViewer(QWidget* parent = nullptr)
		: QMainWindow(parent), work_currentIndex(0) {
		setupUI();
	}

private:
	void setupUI() {

		// 初始化图片列表
		loadImagesFromFolder("../resource/bigImages"); // 替换为你的图片文件夹路径

		//堆叠窗口
		scrollArea = new QScrollArea;
		
		for (const QString& imageFile : imagePaths) {
			QLabel* imageLabel = new QLabel;
			QPixmap pixmap(imageFile);
			imageLabel->setPixmap(pixmap); 
			wid_stacked.push_back(imageLabel); //只添加Label图片
		}
		scrollArea->setWidget(wid_stacked[work_currentIndex]);

		btn_work_next = new QPushButton("下一页");
		btn_work_prev = new QPushButton("上一页");

		btn_work_layout = new QHBoxLayout;
		btn_work_layout->addWidget(btn_work_prev);
		btn_work_layout->addWidget(btn_work_next);

		// 创建主布局
		mainLayout = new QVBoxLayout();
		mainLayout->addWidget(scrollArea);
		mainLayout->addLayout(btn_work_layout);

		connect(btn_work_next, &QPushButton::clicked, this, &ImageViewer::showNextImage);
		connect(btn_work_prev, &QPushButton::clicked, this, &ImageViewer::showPreviousImage);


		// 创建主窗口
		QWidget* centralWidget = new QWidget(this);
		centralWidget->setLayout(mainLayout);
		setCentralWidget(centralWidget);
	}

	void loadImagesFromFolder(const QString& folderPath) {
		QDir dir(folderPath);
		QStringList filters;
		filters << "*.jpg" << "*.jpeg" << "*.png" << "*.bmp"; // 支持的图片格式
		QFileInfoList fileInfoList = dir.entryInfoList(filters, QDir::Files);
		imagePaths.clear();

		for (const QFileInfo& fileInfo : fileInfoList) {
			imagePaths.append(fileInfo.absoluteFilePath());
		}
		work_currentIndex = 0;
		work_prevIndex = 0;
	}

	void updateImageView() {
		if (work_currentIndex >= 0 && work_currentIndex < wid_stacked.count()) {
			wid_stacked[work_prevIndex] = scrollArea->takeWidget();
			scrollArea->setWidget(wid_stacked[work_currentIndex]);
		}
	}

	void showPreviousImage() {
		work_prevIndex = work_currentIndex;
		work_currentIndex--;
		if (work_currentIndex < 0) {
			work_prevIndex = 0;
			work_currentIndex = wid_stacked.count() - 1;
		}
		updateImageView();
	}

	void showNextImage() {
		work_prevIndex = work_currentIndex;
		work_currentIndex++;
		if (work_currentIndex >= wid_stacked.count()) {
			work_currentIndex = 0;
			work_prevIndex = wid_stacked.count() - 1;
		}
		updateImageView();
	}

	QScrollArea* scrollArea;
	QLabel* label;
	QList<QWidget*> wid_stacked;
	QPushButton* btn_work_next = nullptr;
	QPushButton* btn_work_prev = nullptr;
	QVBoxLayout* mainLayout;
	QHBoxLayout* btn_work_layout;
	QStringList imagePaths;
	int work_currentIndex,work_prevIndex;
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	ImageViewer viewer;
	viewer.resize(800, 600);
	viewer.show();

	return app.exec();
}
