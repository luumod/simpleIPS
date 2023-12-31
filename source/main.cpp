﻿#include "Include/Widget/widget.h"
#include <QApplication>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>
#include <QJsonDocument>
#include <QFile>
#include <Windows.h>
//未安装vld的话可以直接删除
//#include <vld.h>
#pragma execution_character_set("utf-8")

class Widget;

int main(int argc,char* argv[])
{
    QApplication a(argc, argv);

    Widget* w = Widget::getInstance();
    w->show();

    //释放单例对象
    QObject::connect(&a, &QApplication::aboutToQuit, [=]() {
        //写入json
        // 创建一个JSON对象并添加数据
        QJsonObject jsonObj;
        jsonObj["win_title"] = w->config.win_title;
        jsonObj["win_location_x"] = w->config.win_location_x;
        jsonObj["win_location_y"] = w->config.win_location_y;
        jsonObj["win_theme"] = w->config.win_theme;
        jsonObj["win_screen_scale"] = w->config.win_screen_scale;


        // 创建JSON文档
        QJsonDocument jsonDoc(jsonObj);

        // 将JSON文档写入文件
        QFile file("config.json");
        if (file.open(QFile::WriteOnly | QFile::Text)) {
            QTextStream out(&file);
            out << jsonDoc.toJson();
            file.close();
        }
        else {
            qWarning() << "打开json文件写入失败!" << file.errorString();
        }
        delete w;
    });

    qInfo()<<"Ui线程：" <<QThread::currentThreadId();
    return a.exec();
}
