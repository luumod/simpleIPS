#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include <QToolButton>

class ColorDialog : public QToolButton
{
    Q_OBJECT

public:
    ColorDialog(QWidget *parent = nullptr);
signals:
    void choice_color(QColor color);
public slots:
    void on_colorBtn_clicked(bool clicked);
};
#endif // WIDGET_H
