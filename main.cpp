/*
 * 作者：Dione
 * 联系QQ：550993637
 * 联系邮箱：550993637@qq.com
 * 博客地址：https://me.csdn.net/u012532263
 */

#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

    return a.exec();
}
