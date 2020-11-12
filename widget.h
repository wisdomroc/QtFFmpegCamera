/*
 * 作者：Dione
 * 联系QQ：550993637
 * 联系邮箱：550993637@qq.com
 * 博客地址：https://me.csdn.net/u012532263
 */

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void slImage(QImage);

private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
