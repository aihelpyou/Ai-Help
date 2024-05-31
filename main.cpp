#include "widget.h"
#include "log.h"

#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qDebug()<<QT_VERSION_STR;
#if (QT_VERSION > QT_VERSION_CHECK(5,13,0))
    //解决界面模糊，缩放比例问题
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif
    //适应高DPI设备
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling,true);

    //解决图片在不同分辨率显示模糊问题
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps,true);

    QApplication::setStyle(QStyleFactory::create("fusion"));

    logInit();
    Widget w;
    w.setWindowIcon(QIcon(":/new/prefix1/logo.ico"));
    w.show();
    return a.exec();
}
