/*
 * 作者：Dione
 * 联系QQ：550993637
 * 联系邮箱：550993637@qq.com
 * 博客地址：https://me.csdn.net/u012532263
 */

#include "widget.h"
#include "ui_widget.h"
#include "my_video_thread.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    MyVideoThread* videoThread = new MyVideoThread;
    connect(videoThread, SIGNAL(capImg(QImage)),this, SLOT(slImage(QImage)));
    videoThread->openCamera();
}

Widget::~Widget()
{
    delete ui;
}


void Widget::slImage(QImage img)
{
    ui->label->setPixmap(QPixmap::fromImage(img));
}
