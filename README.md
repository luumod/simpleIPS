## 简易版图片加工软件

基于 opencv + Qt6.2.4 来完成的。

主要是为了巩固 Qt 与学习 OpenCV 来做的这个软件。

### 点击下载此程序来体验
[一个简单的体验版本](https://github.com/luumod/myPhotoshopApp/releases/tag/v1.0)

### 目前进度

* 图像基础操作
* 图像模糊操作
* 图像阈值化操作
* 图像形态学操作
* 图像连通块分析
* 图像轮廓检测
* 图像直方图操作

###  项目特色

* 功能齐全，基本涵盖了上述的所有操作
* 可视化操作，基于QSlider或者QComboBox，允许用户可以随时指定不同参数的不同数值，以便更好的观察某个操作的具体效果。
* 混合加工模式，允许对同一张图片执行可控制，可视化的混合操作（具体行为因人而异）。

### 项目demo
视频演示地址：

[使用opencv+Qt 自制的简易版图片处理软件](https://www.bilibili.com/video/BV1qN411t7jK/#reply188969413872)

### 开发配置方法
1. 配置好Qt6的环境变量。![image](https://github.com/luumod/myPhotoshopApp/assets/93420580/80b572f9-1e47-452b-b0dd-3ea942643346)
2. 配置好OpenCV的环境变量。![image](https://github.com/luumod/myPhotoshopApp/assets/93420580/3098a71c-c7eb-41cd-8657-b3c5fb1cf5c0)
4. CMakeLists文件中，设置你的OpenCV的安装路径。
5. 删除CMakeLists的倒数后六行（如果你没有安装的话）
6. 重新配置CMakeLists文件

### 待解决bug

* 在create_GUIShow中对滑块设置位置无效，但是setValue值有效，因此引入了两个滑块分别控制亮度升高和降低。
* 图像的掩膜操作的Kernel的设置。
* 在打开工作区的时候，QScrollArea的处理有点太过于复杂，包括work_cutImage函数。

### 联系作者

QQ： 1173012900

邮箱： 

* 1173012900@qq.com
* lyu198691@gmali.com



 
