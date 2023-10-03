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
		: QMainWindow(parent), currentIndex(0) {
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
		scrollArea->setWidget(wid_stacked[currentIndex]);

		next = new QPushButton("下一页");
		prev = new QPushButton("上一页");

		buttonLayout = new QHBoxLayout;
		buttonLayout->addWidget(prev);
		buttonLayout->addWidget(next);

		// 创建主布局
		mainLayout = new QVBoxLayout();
		mainLayout->addWidget(scrollArea);
		mainLayout->addLayout(buttonLayout);

		connect(next, &QPushButton::clicked, this, &ImageViewer::showNextImage);
		connect(prev, &QPushButton::clicked, this, &ImageViewer::showPreviousImage);


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
		currentIndex = 0;
		preIndex = 0;
	}

	void updateImageView() {
		if (currentIndex >= 0 && currentIndex < wid_stacked.count()) {
			wid_stacked[preIndex] = scrollArea->takeWidget();
			scrollArea->setWidget(wid_stacked[currentIndex]);
		}
	}

	void showPreviousImage() {
		preIndex = currentIndex;
		currentIndex--;
		if (currentIndex < 0) {
			preIndex = 0;
			currentIndex = wid_stacked.count() - 1;
		}
		qInfo() << currentIndex;
		updateImageView();
	}

	void showNextImage() {
		preIndex = currentIndex;
		currentIndex++;
		if (currentIndex == 9) {
			qInfo() << "dda";
		}
		if (currentIndex >= wid_stacked.count()) {
			currentIndex = 0;
			preIndex = wid_stacked.count() - 1;
		}
		qInfo() << currentIndex;
		updateImageView();
	}

	QScrollArea* scrollArea;
	QLabel* label;
	QList<QWidget*> wid_stacked;
	QPushButton* next = nullptr;
	QPushButton* prev = nullptr;
	QVBoxLayout* mainLayout;
	QHBoxLayout* buttonLayout;
	QStringList imagePaths;
	int currentIndex,preIndex;
};

int main(int argc, char* argv[]) {
	QApplication app(argc, argv);

	ImageViewer viewer;
	viewer.resize(800, 600);
	viewer.show();

	return app.exec();
}
