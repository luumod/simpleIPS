#ifndef DEPRECATED_H
#define DEPRECATED_H
#include <QtWidgets>
/**
 * 已经弃用的所有函数
 *
 */


//Dialog: 创建n个滑块和输入框
/**
     * @brief 对于GUI窗口的封装：可以创建n个QSlider
     * @tparam T 				最小值，最大值，步长的数据类型
     * @tparam Type 			create_Edit_hLayout 的模板类型
     * @param  ls_slider       	窗口中所有的QSlider实体，把这些QSlider放到一个QList中进行管理
     * @param  low              n个QSlider分别对应的最小值
     * @param  high             n个QSlider分别对应的最大值
     * @param  step            	n个QSlider分别对应的步长
     * @param  objectName       n个QSlider分别对应的类名
     * @param  lab_name         n个QSlider分别对应的提示标签的名字
     * @param  slotFunction     n个QSlider分别对应槽函数，连接其 SliderMoved 信号
     * @param  edit            	true：表示允许有输入功能 ，false表示不允许输入
     * @param  filter          	参见 create_Edit_hLayout
     * @param  text             参见 create_Edit_hLayout
     * @param  t                参见 create_Edit_hLayout
     * @return QBoxLayout* 		返回的是包含n个QSlider的窗口布局
     *
     * @todo 实际上直接返回QWidget就行
     */
template<typename T, typename Type>
QBoxLayout* createDialog_nSlider_GUItemplate(
    QList<QSlider*>& ls_slider,
    QList<T> low,
    QList<T> high,
    QList<T> step,
    QList< QString> objectName,
    QList< QString> lab_name,
    QList<std::function<void(int)>> slotFunction,
    bool edit,
    const QString& filter,
    const QString& text,
    Type* t
    ){
    QVBoxLayout* vlayout = new QVBoxLayout;
    for (int i = 0; i < ls_slider.size(); i++) {
        ls_slider[i] = new QSlider(Qt::Horizontal);
        ls_slider[i]->setRange(low[i], high[i]);
        ls_slider[i]->setSingleStep(step[i]);
        ls_slider[i]->setObjectName(objectName[i]);
        ls_slider[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        //传递一个槽函数
        connect(ls_slider[i], &QSlider::sliderMoved,
                this, slotFunction[i]);

        QHBoxLayout* hlayout = new QHBoxLayout;
        hlayout->addWidget(new QLabel(lab_name[i]));
        hlayout->addWidget(ls_slider[i]);

        vlayout->addLayout(hlayout);
    }
    //----------------------------
    //具有可输入功能，添加一个输入框
    if (edit) {
        vlayout->addSpacing(10);
        vlayout->addLayout(create_Edit_hLayout(filter, text, t));
    }
    return vlayout;
}

/**
     * @brief 对于GUI窗口的封装：可以创建n个QComboBox
     *
     * @see 具体内容请参见：createDialog_nSlider_GUItemplate 其实现基本一致
     */
QBoxLayout* createDialog_nComBox_GUItemplate(
    QList<QComboBox*>& ls_combox,
    QList<QStringList> ls_item,
    QList< QString> objectName,
    QList< QString> lab_name,
    QList<std::function<void(int)>> slotFunction)
{
    QVBoxLayout* vlayout = new QVBoxLayout;
    for (int i = 0; i < ls_combox.size(); i++) {
        ls_combox[i] = new QComboBox;
        ls_combox[i]->setEditable(false);
        for (auto& item : ls_item[i]) {
            ls_combox[i]->addItem(item);
        }
        ls_combox[i]->setObjectName(objectName[i]);
        ls_combox[i]->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

        //传递一个槽函数
        connect(ls_combox[i], &QComboBox::activated,
                this, slotFunction[i]);

        QHBoxLayout* hlayout = new QHBoxLayout;
        hlayout->addWidget(new QLabel(lab_name[i]));
        hlayout->addWidget(ls_combox[i]);

        vlayout->addLayout(hlayout);
    }
    return vlayout;
}

#endif // DEPRECATED_H
