#ifndef CONFIG_H_
#define CONFIG_H_

#include <QString>
#include <QDebug>

struct ExeConfig {
	QString win_title = "简易图片处理软件";
	int win_location_x = 50, win_location_y = 50;
	QString win_theme = "light";
	double win_screen_scale = 1.0;
	friend QDebug operator<<(QDebug debug, const ExeConfig& oth) {
		QDebugStateSaver saver(debug);
		debug << "win_title: " << oth.win_title << '\n';
		debug << "win_location_x: " << oth.win_location_x << '\n';
		debug << "win_location_y: " << oth.win_location_y << '\n';
		debug << "win_theme: " << oth.win_theme << '\n';
		debug << "win_screen_scale" << oth.win_screen_scale << '\n';
		return debug;
	}
};

#endif // !CONFIG_H_
