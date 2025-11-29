#ifndef WIDGET_H
#define WIDGET_H

#include <QLabel>
#include <QMenu>
#include <QWidget>
#include <qnetworkreply.h>
#include "citycodeutils.h"
#include "day.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    Day days[7];  // 存储每天的天气数据
    QList<QLabel *> mWeekList;
    QList<QLabel *> mDateList;
    QList<QLabel *> mIconList;
    QList<QLabel *> mWeaTypeList;
    QList<QLabel *> mAirqList;
    QList<QLabel *> mFxList;
    QList<QLabel *> mFlList;

    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event) override;           // 捕获鼠标点击事件
    void mouseMoveEvent(QMouseEvent *event) override;   // 鼠标移动事件
    //void paintEvent(QPaintEvent *event) override;       // 画图事件
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    /*
     QNetworkAccessManager *manager = new QNetworkAccessManager(this);
     connect(manager, &QNetworkAccessManager::finished,
             this, &MyClass::replyFinished);
          manager->get(QNetworkRequest(QUrl("http://qt-project.org")));
         */
    void readHttpReply(QNetworkReply *reply);

private slots:
    void on_pushButton_clicked();

    void on_lineEditCity_returnPressed();

private:
    Ui::Widget *ui;
    QMenu *menuQuit;   // 鼠标右键后显示的退出按钮
    QPoint mOffset;  // 偏移值
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QString strUrl;  // url地址
    CityCodeUtils cityCodeUtils;  // 自定义的C++类
    QMap<QString,QString> mTypeMap;  // 天气和天气图标路径进行绑定
    void parseWeatherJsonData(QByteArray rawData);     // 解析v61版本的小时天气数据
    void parseWeatherJsonDataNew(QByteArray rawData); // 解析7天天气的JSON数据解析 v9版本7天天气数据
    void updateUI();
    void drawTempLineHigh();  // 最高温折线图绘画函数
    void drawTempLineLow();   // 最低温折线图绘画函数
};
#endif // WIDGET_H
