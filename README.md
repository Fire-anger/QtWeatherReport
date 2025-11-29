# QtWeatherReport
基于Qt开发的天气预报
## 3.Qt天气预报开发

### 3.1 UI设置的注意事项

般用的都是 Widge

![image-20251125122622453](D:/TyporaPicture/image-20251125122622453.png)

![image-20251125122601028](D:/TyporaPicture/image-20251125122601028.png)



有些样式可以在帮助文档中查找

![image-20251125133409000](D:/TyporaPicture/image-20251125133409000.png)

用 Ctrl+ f 进行搜索就行了

![image-20251125133526999](D:/TyporaPicture/image-20251125133526999.png)



固定大小和去除windows上边框

![image-20251125211204095](D:/TyporaPicture/image-20251125211204095.png)



### 3.2 退出功能开发

当鼠标右键时，产生一个退出界面 QMenu，通过查找发现 QMenu 和 QAction 有关系，所以又创建 QAction，具体的看文档

![image-20251126111649460](D:/TyporaPicture/image-20251126111649460.png)

![image-20251126111430025](D:/TyporaPicture/image-20251126111430025.png)

![image-20251126111722857](D:/TyporaPicture/image-20251126111722857.png)

### 3.3 移动窗口

![image-20251126204746024](D:/TyporaPicture/image-20251126204746024.png)

![image-20251126210229091](D:/TyporaPicture/image-20251126210229091.png)

### 3.4 在Qt中通过网络请求获取天气数据

天气API接口地址： [实时天气预报api 24小时天气预报接口 生活指数预报 空气质量预报](http://www.tianqiapi.com/index/doc?version=v62)

在Qt中使用 network 网络相关的内容，需要在 CMake中或者 .pro 文件中进行配置，我这里是自动配置的

![image-20251127143934224](D:/TyporaPicture/image-20251127143934224.png)

**网络请求**

![image-20251127151935211](D:/TyporaPicture/image-20251127151935211.png)

**错误处理**

![image-20251127153915821](D:/TyporaPicture/image-20251127153915821.png)

```c++
    // 2. 网络请求获取天气数据
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    // 验证是否创建manager成功
    // connect(manager, &QNetworkAccessManager::finished,this, [=](){
    //     qDebug() << "manager finish!==========";
    // });
    QUrl urlItBoy("http://gfeljm.tianqiapi.com/free/v2030?city=&cityid=&adcode=130200000000&appid=22429431&appsecret=x7Gwi04U&lng=&lat=&aqi=&hours=");
    QUrl urlTianQi("http://gfeljm.tianqiapi.com/api?unescape=1&version=v63&appid=22429431&appsecret=x7Gwi04U");
    QNetworkRequest res(urlItBoy);
    // 发起get请求 返回值为 QNetworkReply*
    reply = manager->get(res);
    // 网络请求后进行数据读取，给请求后的返回值进行信号与槽绑定
    connect(reply,&QNetworkReply::finished,this,&Widget::readHttpReply);

void Widget::readHttpReply()
{
    // 获取状态码
    int resCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << resCode;
    if(reply->error() == QNetworkReply::NoError && resCode == 200){
        // 大多数服务器返回的编码格式是utf-8的
        QByteArray data = reply->readAll();
        qDebug() << QString::fromUtf8(data);
    }else{
        qDebug() << "请求失败: " << reply->errorString();
        //QMessageBox::warning(this,"错误", "网络请求失败", QMessageBox::Ok);
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("网络请求失败");
        mes.setStyleSheet("QPushButton {color: red}");
        mes.setStandardButtons(QMessageBox::Ok);
        mes.exec();
    }

}
```

### 3.5 Qt 中Json的使用

工程名为 WeatherQTJSON

Json 处理的函数

![image-20251127163538500](D:/TyporaPicture/image-20251127163538500.png)

**JSON对象：QJsonObject 、JSON数组：QJsonArray、用于转换JSON：QJsonDocument、写入文件中需要这个对象：QByteArray**

```cpp
#include "widget.h"
#include "./ui_widget.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonObject>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1.Json 对象 QJsonObject
    QJsonObject rootObj;
    rootObj["city"] = "1010100";
    rootObj["date"] = "2525-11-27";
    rootObj["weather"] = "暴雨";
    rootObj["tmp"] = 10;

    // 2.json 数组
    QJsonArray jsonArray;
    jsonArray.append("data1");
    jsonArray.append("data2");
    jsonArray.append("data3");
    jsonArray.append(1);
    // 添加到 QJsonObject 中
    rootObj["testArray"] = jsonArray;

    // 3.Json(对象) 中添加 Json对象
    QJsonObject alarmObj;
    alarmObj["alarmType"] = "雪灾";
    alarmObj["alarmLevel"] = "黄色";
    alarmObj["alarmTitle"] = "福州市警告避免外出";
    // 添加到 Json 中
    rootObj["alarm"] = alarmObj;

    // 4.数组中的元素是 Json 对象
    QJsonObject day0;
    day0["day"] = "星期一";
    day0["wed"] = "晴天";
    day0["tmp"] = 5.7;

    QJsonObject day1;
    day1["day"] = "星期二";
    day1["wed"] = "阴天";
    day1["tmp"] = 6.8;

    QJsonObject day2;
    day2["day"] = "星期三";
    day2["wed"] = "晴天";
    day2["tmp"] = 5.9;

    QJsonArray dayArray;
    dayArray.append(day0);
    dayArray.append(day1);
    dayArray.append(day2);
    rootObj["days"] = dayArray;

    // 5.通过 QJsonDocument 这个类把 JSON 数据转换成 QByteArray
    QJsonDocument jsonDoc(rootObj);

    // 6.将 QJsonDocument 转成 QByteArray，通过 QByteArray 写入到文件中
    QByteArray jsonArray1 = jsonDoc.toJson();

    QFile file("C:/Users/33150/Desktop/test.json");
    file.open(QIODevice::WriteOnly);
    file.write(jsonArray1);
    file.close();
}
```

结果如下

```json
{
    "alarm": {
        "alarmLevel": "黄色",
        "alarmTitle": "福州市警告避免外出",
        "alarmType": "雪灾"
    },
    "city": "1010100",
    "date": "2525-11-27",
    "days": [
        {
            "day": "星期一",
            "tmp": 5.7,
            "wed": "晴天"
        },
        {
            "day": "星期二",
            "tmp": 6.8,
            "wed": "阴天"
        },
        {
            "day": "星期三",
            "tmp": 5.9,
            "wed": "晴天"
        }
    ],
    "testArray": [
        "data1",
        "data2",
        "data3",
        1
    ],
    "tmp": 10,
    "weather": "暴雨"
}

```



### 3.6 JSON数据解析

工程名为 ParseJSON

```cpp
#include "widget.h"
#include "./ui_widget.h"

#include <QJsonDocument>
#include <qfile.h>
#include <qjsonarray.h>
#include <qjsonobject.h>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    // 1.读取JSON文件保存到QByteArray中
    QFile file("C:/Users/33150/Desktop/test.json");
    file.open(QIODevice::ReadOnly);
    QByteArray rawData = file.readAll();
    file.close();

    // 2.将文档中保存的Json数据转为QJsonDocument
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if(!jsonDoc.isNull() && jsonDoc.isObject()){  // 判断是否为空和是否是一个Json对象
        // 3.将 QJsonDocument 转为 Json 对象 QJsonObject
        QJsonObject jsonRoot = jsonDoc.object();

        // 4.普通Json数据
        QString strW = jsonRoot["weather"].toString();
        int tempreture = jsonRoot["tmp"].toInt();

        qDebug() << strW;
        qDebug() << tempreture; // QString::number(tempreture)

        // 5.解析数组，元素是字符串和整型
        if(jsonRoot.contains("testArray") && jsonRoot["testArray"].isArray()){ // 如果testArray存在，且为数组
            QJsonArray testArray = jsonRoot["testArray"].toArray();
            // for(QJsonValue val : testArray) 之前是写成这样的，有警告，问AI说改成这样
            for(const QJsonValue &val : std::as_const(testArray)){
                //QJsonValue::Type t = val.type();
                switch (val.type()) {
                case QJsonValue::String:
                    qDebug() << val.toString();
                    break;
                case QJsonValue::Double:
                    qDebug() << QString::number(val.toDouble());
                    break;
                case QJsonValue::Object:
                    break;
                default:
                    break;
                }
            }
        }

        // 6.解析对象
        if(jsonRoot.contains("alarm") && jsonRoot["alarm"].isObject()){
            QJsonObject alarmObj = jsonRoot["alarm"].toObject();
            qDebug() << alarmObj["alarmLevel"].toString();
            qDebug() << alarmObj["alarmTitle"].toString();
            qDebug() << alarmObj["alarmType"].toString();
        }

        // 7.解析数组，元素是对象
        if(jsonRoot.contains("days") && jsonRoot["days"].isArray()){
            QJsonArray daysArray = jsonRoot["days"].toArray();
            for(const QJsonValue &val : std::as_const(daysArray)){
                if(val.isObject()){ // 或者 val.type() == QJsonValue::Object
                    QJsonObject obj = val.toObject();
                    qDebug() << obj["day"].toString();
                    qDebug() << QString::number(obj["tmp"].toDouble());
                    qDebug() << obj["wed"].toString();
                }
            }
        }
    }

}


```



### 3.7 渲染响应数据修改网络请求处理方式

修改了一下网络的请求方式，可以和上面的网络请求进行对照

![image-20251128113653294](D:/TyporaPicture/image-20251128113653294.png)

![image-20251128113739261](D:/TyporaPicture/image-20251128113739261.png)

### 3.8 QMap解决天气的Bug

搜索时有些城市带有 "市" "县" ，如果我们按照上面的写法会导致搜索不到，并且每次搜索时都需要打开文件和关闭文件，效率比较低，因此我们需要来解决这个问题，使用QMap(相当于C++的Map，通过find()函数来查找城市名称)来解决

创建一个C++类 `citycodeutils.cpp` 来解决这个问题

![image-20251128144509762](D:/TyporaPicture/image-20251128144509762.png)

QMap 的 constBegin() 和 constEnd() 对于C++的 begin() 和 end()

![image-20251128145529187](D:/TyporaPicture/image-20251128145529187.png)

关于市 、县、区的处理方法，由于find不支持模糊查询，只能通过这种方式来实现模糊查询

![image-20251128151611065](D:/TyporaPicture/image-20251128151611065.png)

### 3.9 刷新天气图标

通过 QMap 对天气和图片路径进行绑定

![image-20251128153806462](D:/TyporaPicture/image-20251128153806462.png)

### 3.10 获取7天天气预报

创建一个 C++ 类 名为 `day.cpp`  注意这里类名记得首字母要大写

![image-20251128155146908](D:/TyporaPicture/image-20251128155146908.png)

Day 类中存放当天的天气属性

![image-20251128164251066](D:/TyporaPicture/image-20251128164251066.png)

### 3.11 更新7天天气UI显示

对应的UI

![image-20251128164914571](D:/TyporaPicture/image-20251128164914571.png)

![image-20251128194438876](D:/TyporaPicture/image-20251128194438876.png)

### 3.12 用事件过滤器在子控件上绘图

#### 3.12.1 视频的方法

**老师是以这个为基准计算点的坐标的**

![image-20251128212611522](D:/TyporaPicture/image-20251128212611522.png)

![image-20251128201047863](D:/TyporaPicture/image-20251128201047863.png)

```Cpp
// 视频的代码
// 最高温折线图绘画函数
void Widget::drawTempLineHigh()
{
    // 在 widget0404 上画
    QPainter painter(ui->widget0404);
    painter.setBrush(Qt::yellow);  // 画实心点
    painter.setRenderHint(QPainter::Antialiasing, true); // 添加抗齿，使曲线更好看
    painter.setPen(Qt::yellow);

    // 计算平均温度
    int ave;
    int sum = 0;

    for(int i = 0; i < 6; i++){
        sum += days[i].mTempHigh.toInt();
    }
    ave = sum / 6;


    QPoint points[6];  // 定义折线图的6个点
    int offSet = 0;   // 偏移值
    int middle = ui->widget0404->height()/2;  // widget0404空间的获取中间y位置
    // 画出6个点
    for(int i = 0; i < 6; i++){
        points[i].setX(mAirqList[i]->x() + mAirqList[i]->width()/2);
        offSet = (days[i].mTempHigh.toInt() - ave) * 3; // 如果为正，说明当天温度大于平均值，那么y值应该低于middle；否则，y值高于middle
        points[i].setY(middle - offSet);

        // 画出圈圈
        painter.drawEllipse(QPoint(points[i]),3,3);
        // 画出当天温度
        painter.drawText(points[i].x() - 15, points[i].y() - 15, days[i].mTempHigh + "°");
    }
    // 画5条线
    for(int i = 0; i < 5; i++){
        painter.drawLine(points[i],points[i+1]);
    }
}
```

#### 3.12.2 我的方法

就利用 widget0405 和  widget0406 来锁定六个点的 x 和 y 的坐标值

有两种方法都可以画出来折线图，我以为视频的方法不行的
遇到一个问题，就是最高低的温度数字无法显示到折线上，原因是需要在解析完响应数据后调用这个重新渲染这两个组件，整了好久才解决

![image-20251128212118252](D:/TyporaPicture/image-20251128212118252.png)

```C++
// 我的代码，就是 widget0404 左上角为 0,0 这样的

// 最高温折线图绘画函数
void Widget::drawTempLineHigh()
{
    // 在 widget0404 上画
    QPainter painter(ui->widget0404);
    painter.setBrush(Qt::yellow);  // 画实心点
    painter.setRenderHint(QPainter::Antialiasing, true); // 添加抗齿，使曲线更好看
    painter.setPen(Qt::yellow);

    // 计算平均温度
    int ave;
    int sum = 0;

    for(int i = 0; i < 6; i++){
        sum += days[i].mTempHigh.toInt();
    }
    ave = sum / 6;


    QPoint points[6];  // 定义折线图的6个点
    int offSet = 0;   // 偏移值

    /* 2. 画布尺寸 */
    int w = ui->widget0404->width();
    int h = ui->widget0404->height();
    int dx = w / 6;               // 水平间距
    int midY = h / 2;			 // 中间高度
    for (int i = 0; i < 6; ++i) {
        points[i].setX(dx * i + dx / 2);
        offSet = (days[i].mTempHigh.toInt() - ave) * 3;
        points[i].setY(midY - offSet);

        // 画出6个点
        painter.drawEllipse(QPoint(points[i]), 3, 3);
        // 画出当天温度
        painter.drawText(points[i].x() - 15, points[i].y() - 10, days[i].mTempHigh + "°");
    }
    // 画5条线
    for(int i = 0; i < 5; i++){
        painter.drawLine(points[i],points[i+1]);
    }
}
// 事件过滤
bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widget0404 && event->type() == QEvent::Paint){
        drawTempLineHigh(); // 调用画图函数
        return true;  // 表示事件已被处理，不需要向下传递
    }
    if(watched == ui->widget0405 && event->type() == QEvent::Paint){
        drawTempLineLow(); // 调用画图函数
        return true;  // 表示事件已被处理，不需要向下传递
    }

    return QWidget::eventFilter(watched, event);
}
```

最终效果

500 × 900 的大小

![image-20251128215708575](D:/TyporaPicture/image-20251128215708575.png)
