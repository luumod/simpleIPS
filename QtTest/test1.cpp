#include <QApplication>
#include <QPushButton>

// 建造者模式用于创建按钮
class ButtonBuilder {
public:
    ButtonBuilder(const QString& text) {
        button_ = new QPushButton(text);
    }

    ButtonBuilder& setGeometry(int x, int y, int width, int height) {
        button_->setGeometry(x, y, width, height);
        return *this;
    }

    ButtonBuilder& setEnabled(bool enabled) {
        button_->setEnabled(enabled);
        return *this;
    }

    QPushButton* build() {
        return button_;
    }

private:
    QPushButton* button_;
};

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    // 使用建造者模式创建一个按钮
    QPushButton* button1 = ButtonBuilder("Click Me")
        .setGeometry(100, 100, 200, 50)
        .setEnabled(true)
        .build();

    // 使用建造者模式创建另一个按钮
    QPushButton* button2 = ButtonBuilder("Submit")
        .setGeometry(100, 200, 200, 50)
        .setEnabled(false)
        .build();

    button1->show();
    button2->show();

    return app.exec();
}
