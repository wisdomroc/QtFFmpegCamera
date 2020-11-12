/*
 * 作者：Dione
 * 联系QQ：550993637
 * 联系邮箱：550993637@qq.com
 * 博客地址：https://me.csdn.net/u012532263
 */

#ifndef MYVIDEOTHREAD_H
#define MYVIDEOTHREAD_H

#include <QThread>
#include <QImage>

class MyVideoThread : public QThread
{
    Q_OBJECT
public:
    explicit MyVideoThread(QObject *parent = Q_NULLPTR);
    void openCamera();

protected:
    virtual void run();

Q_SIGNALS:
    void capImg(QImage);

private:
    QString _cameraName;

};

#endif // MYVIDEOTHREAD_H
