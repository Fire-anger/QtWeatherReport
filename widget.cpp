#include "widget.h"
#include "./ui_widget.h"

#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <Qurl>
#include <warning.h>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QJsonArray>
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(500,900); // 固定大小  406,787  463,823
    setWindowFlag(Qt::FramelessWindowHint); // 去除windows上边框

    // 1.堆区创建一个QMenu菜单组件
    menuQuit = new QMenu(this);
    // 设置菜单项文字颜色
    menuQuit->setStyleSheet("QMenu::item {color: white}");
    // 为菜单组件添加一个动作   通过案例，发现 QMenu 和 QAction 会产生关系，所以又创建一个 QAction 出来
    QAction *closeAct = new QAction(QIcon(":/res/退出.png"),tr("退出"),this);
    // 添加 QAction 到 QMenu ，要不然exec执行起来是空的，没有任何作用
    menuQuit->addAction(closeAct);
    // 用户选择菜单项并点击后回调函数，退出程序 当按下退出时会触发一个 QMenu::triggered 的信号，通过绑定信号与槽 在匿名函数中执行退出操作即可
    connect(menuQuit, &QMenu::triggered, this,[=](){
        this->close();
    });

    // qDebug() << this->pos(); // 这个打印出来的结果是 QPoint(0,0) 因为窗口还没有显示出来

    // 2. 网络请求获取天气数据
    manager = new QNetworkAccessManager(this);
    // 验证是否创建manager成功
    // connect(manager, &QNetworkAccessManager::finished,this, [=](){
    //     qDebug() << "manager finish!==========";
    // });

    //strUrl = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v61&appid=userid&appsecret=x7Gwi04U"; // v61版本的小时天气数据
    strUrl = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v9&appid=userid&appsecret=x7Gwi04U"; // v9版本7天天气数据
    QUrl urlTianQi(strUrl);
    QNetworkRequest res(urlTianQi);
    // 发起get请求 返回值为 QNetworkReply*
    reply = manager->get(res);
    // 网络请求后进行数据读取，给请求后的返回值进行信号与槽绑定
    connect(manager,&QNetworkAccessManager::finished,this,&Widget::readHttpReply);

    // 初始化 QList<QLabel *> 属性
    mWeekList << ui->labelday0 << ui->labelday1
              << ui->labelday2 << ui->labelday3
              << ui->labelday4 << ui->labelday5;

    mDateList << ui->labelDate0 << ui->labelDate1
              << ui->labelDate2 << ui->labelDate3
              << ui->labelDate4 << ui->labelDate5;

    mIconList << ui->labelWeatherIcon0 << ui->labelWeatherIcon1
              << ui->labelWeatherIcon2 << ui->labelWeatherIcon3
              << ui->labelWeatherIcon4 << ui->labelWeatherIcon5;

    mWeaTypeList << ui->lbweatherTypeData0 << ui->lbweatherTypeData1
                 << ui->lbweatherTypeData2 << ui->lbweatherTypeData3
                 << ui->lbweatherTypeData4 << ui->lbweatherTypeData5;

    mAirqList << ui->labelairq0 << ui->labelairq1
              << ui->labelairq2 << ui->labelairq3
              << ui->labelairq4 << ui->labelairq5;

    mFxList << ui->labelFX0 << ui->labelFX1
            << ui->labelFX2 << ui->labelFX3
            << ui->labelFX4 << ui->labelFX5;

    mFlList << ui->labelFL0 << ui->labelFL1
            << ui->labelFL2 << ui->labelFL3
            << ui->labelFL4 << ui->labelFL5;

    //根据keys,设置icon的路径
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu. png");
    mTypeMap.insert("暴雨到大暴雨",":/res/type/BaoYuDaoDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到特大暴雨",":/res/type/DaBaoYuDaoTeDaBaoYu.png");
    mTypeMap.insert("大到暴雪",":/res/type/DaDaoBaoXue.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴有冰雹",":/res/type/LeiZhenYuBanYouBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShaChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("undefined",":/res/type/undefined.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小到中雪",":/res/type/XiaoDaoZhongXue.png");
    mTypeMap.insert("小到中雨",":/res/type/XiaoDaoZhongYu.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("雪",":/res/type/Xue.png");
    mTypeMap.insert("扬沙",":/res/type/YangSha.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨",":/res/type/Yu.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("中到大雪",":/res/type/ZhongDaoDaXue.png");
    mTypeMap.insert("中到大雨",":/res/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");
    mTypeMap.insert("浮尘",":/res/type/FuChen.png");

    // 给控件安装事件过滤器
    ui->widget0404->installEventFilter(this);
    ui->widget0405->installEventFilter(this);
}

// // 解析v61版本的小时天气数据
void Widget::parseWeatherJsonData(QByteArray rawData)
{
    // 将Json转为QJsonDocument对象
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if(!jsonDoc.isNull() && jsonDoc.isObject()){
        // 得到json对象
        QJsonObject objRoot = jsonDoc.object();
        // 解析日期和星期
        QString date = objRoot["date"].toString();
        QString week = objRoot["week"].toString();
        ui->labelCurrentDate->setText(date + "  " + week);
        // 解析城市
        QString city = objRoot["city"].toString();
        ui->labelCity->setText(city + "市");
        // 解析当前温度
        QString tem = objRoot["tem"].toString();
        ui->labelTemp->setText(tem + "℃");
        // 解析天气
        ui->labelWeatherType->setText(objRoot["wea"].toString());
        ui->labelWeatherIcon->setPixmap(mTypeMap[objRoot["wea"].toString()]);
        // 解析温度范围
        ui->labelTempRange->setText(objRoot["tem2"].toString() + "~" + objRoot["tem1"].toString());
        // 解析感冒指数
        ui->labelGanmao->setText(objRoot["air_tips"].toString());
        // 解析风向
        ui->labelFXType->setText(objRoot["win"].toString());
        // 解析风力
        ui->labelFXType_2->setText(objRoot["win_speed"].toString());
        // 解析PM2.5
        ui->labelPM25Data->setText(objRoot["air_pm25"].toString());
        // 解析湿度
        ui->labelShiiduData->setText(objRoot["humidity"].toString());
        // 解析空气质量
        ui->labelAirData->setText(objRoot["air_level"].toString());
    }
}

// 解析7天天气的JSON数据解析 v9版本7天天气数据
void Widget::parseWeatherJsonDataNew(QByteArray rawData)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if(!jsonDoc.isNull() && jsonDoc.isObject()){
        QJsonObject jsonRoot = jsonDoc.object();
        days[0].mCity = jsonRoot["city"].toString();
        days[0].MPm25 = jsonRoot["aqi"].toObject()["pm25"].toString();
        if(jsonRoot.contains("data") && jsonRoot["data"].isArray()){
            QJsonArray weaArray = jsonRoot["data"].toArray();
            for(int i = 0; i < weaArray.size(); i++){
                // 取出数组中的每一个Json对象
                QJsonObject obj = weaArray[i].toObject();
                days[i].mDate = obj["date"].toString();
                days[i].mWeek = obj["week"].toString();
                days[i].mTemp = obj["tem"].toString();
                days[i].mWeathType = obj["wea"].toString();
                days[i].mTempLow = obj["tem2"].toString();
                days[i].mTempHigh = obj["tem1"].toString();
                days[i].mFx = obj["win"].toArray()[0].toString();
                days[i].mFl = obj["win_speed"].toString();
                days[i].mAirq = obj["air_level"].toString();
                days[i].mTips = obj["index"].toArray()[3].toObject()["desc"].toString();
                days[i].mShiDu = obj["humidity"].toString();

            }
        }
    }
    updateUI();
    ui->widget0404->update();   // 最高温折线
    ui->widget0405->update();   // 最低温折线
}

// 更新UI
void Widget::updateUI()
{
    QPixmap pixmap;
    // 解析日期和星期
    ui->labelCurrentDate->setText(days[0].mDate + "  " + days[0].mWeek);
    // 解析城市
    ui->labelCity->setText(days[0].mCity + "市");
    // 解析当前温度
    ui->labelTemp->setText(days[0].mTemp + "℃");
    // 解析温度范围
    ui->labelTempRange->setText(days[0].mTempLow + "℃" + "~" + days[0].mTempHigh + "℃");
    // 解析天气
    ui->labelWeatherType->setText(days[0].mWeathType);
    ui->labelWeatherIcon->setPixmap(mTypeMap[days[0].mWeathType]);
    // 解析感冒指数
    ui->labelGanmao->setText(days[0].mTips);
    // 解析风向
    ui->labelFXType->setText(days[0].mFx);
    // 解析风力
    ui->labelFXType_2->setText(days[0].mFl);
    // 解析PM2.5
    ui->labelPM25Data->setText(days[0].MPm25);
    // 解析湿度
    ui->labelShiiduData->setText(days[0].mShiDu);
    // 解析空气质量
    ui->labelAirData->setText(days[0].mAirq);

    for(int i = 0; i < 6; i++){
        // 处理星期
        if(i >= 3){
            mWeekList[i]->setText(days[i].mWeek);
        }
        // 处理日期
        QStringList dayList = days[i].mDate.split("-");
        mDateList[i]->setText(dayList.at(1) + "-" + dayList.at(2));

        // 处理图片
        // 缩放图片大小和label大小能匹配
        int index = days[i].mWeathType.indexOf("转");
        if(index != -1){
            // 比如多云转晴  就取多云的图片  pixmap是QPixmap类的
            pixmap = mTypeMap[days[i].mWeathType.left(index)];
        }else{
            pixmap = mTypeMap[days[i].mWeathType];
        }
        pixmap = pixmap.scaled(mIconList[i]->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        mIconList[i]->setMaximumHeight(59);  // 设置图片高度为59  也可以在UI里面定死
        mIconList[i]->setMaximumWidth(74);   // 设置图片宽度为74
        mIconList[i]->setPixmap(pixmap);

        mWeaTypeList[i]->setText(days[i].mWeathType);

        QString airQ = days[i].mAirq;
        mAirqList[i]->setText(airQ);
        if(airQ == "优"){
            mAirqList[i]->setStyleSheet("background-color:rgb(0, 142, 0);border-radius: 8px;color:rgb(255,255,255);");
        }else if(airQ == "良"){
            mAirqList[i]->setStyleSheet("background-color:rgb(166, 111, 0);border-radius: 8px;color:rgb(255,255,255);");
        }else if(airQ == "轻度污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(255, 199, 199);border-radius: 8px;color:rgb(255,255,255);");
        }else if(airQ == "中度污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(255, 17, 17);border-radius: 8px;color:rgb(255,255,255);");
        }else if(airQ == "重度污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(153, 0, 0);border-radius: 8px;color:rgb(255,255,255);");
        }


        // 风向中带有转的也只取前面部分  3~4级转<3级别 变成 3~4级
        index = days[i].mFl.indexOf("转");
        if(index != -1){
            mFxList[i]->setText(days[i].mFl.left(index));
        }else{
            mFxList[i]->setText(days[i].mFl);
        }

        mFlList[i]->setText(days[i].mFx);
    }
}


// 读取请求返回的结果
void Widget::readHttpReply(QNetworkReply *reply)
{
    // 获取状态码
    int resCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    qDebug() << resCode;
    if(reply->error() == QNetworkReply::NoError && resCode == 200){
        // 大多数服务器返回的编码格式是utf-8的
        QByteArray data = reply->readAll();
        //parseWeatherJsonData(data);
        parseWeatherJsonDataNew(data);
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

Widget::~Widget()
{
    delete ui;
}

// 鼠标点击事件
void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){  // 如果是鼠标右键按下
        //qDebug() << "Right Mouse clicked!";
        menuQuit->exec(QCursor::pos());  // 弹出一个 QMenu 窗口用于提示是否退出，QCursor::pos() 表示出现位置在当前的鼠标位置
    }

    // 鼠标当前位置 event->globalPos()
    // 窗口的当前位置 this->pos()
    if(event->button() == Qt::LeftButton){  // 如果是鼠标左键按下
        // qt6中globalPos被弃用，换成globalPosition，精度更高
        // qDebug() << "Left Mouse clicked!" << event->globalPos() << this->pos();
        mOffset = event->globalPos() - this->pos();  // 得到偏移位置
    }
}

// 鼠标左键按下后的移动，导致这个事件被调用，设置窗口的新位置
void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);  // 移动后的鼠标位置减去偏移位置就是窗口左上角位置
}

// 最高温折线图绘画函数
// void Widget::drawTempLineHigh()
// {
//     // 在 widget0404 上画
//     QPainter painter(ui->widget0404);
//     painter.setBrush(Qt::yellow);  // 画实心点
//     painter.setRenderHint(QPainter::Antialiasing, true); // 添加抗齿，使曲线更好看
//     painter.setPen(Qt::yellow);

//     // 计算平均温度
//     int ave;
//     int sum = 0;

//     for(int i = 0; i < 6; i++){
//         sum += days[i].mTempHigh.toInt();
//     }
//     ave = sum / 6;


//     QPoint points[6];  // 定义折线图的6个点
//     int offSet = 0;   // 偏移值
//     //int middle = ui->widget0404->height()/2;  // widget0404空间的获取中间y位置
//     // 画出6个点
//     // for(int i = 0; i < 6; i++){
//     //     points[i].setX(mAirqList[i]->x() + mAirqList[i]->width()/2);
//     //     offSet = (days[i].mTempHigh.toInt() - ave) * 3; // 如果为正，说明当天温度大于平均值，那么y值应该低于middle；否则，y值高于middle
//     //     points[i].setY(middle - offSet);

//     //     // 画出圈圈
//     //     painter.drawEllipse(QPoint(points[i]),3,3);
//     //     // 画出当天温度
//     //     painter.drawText(points[i].x() - 15, points[i].y() - 15, days[i].mTempHigh + "°");
//     // }
//     /* 3. 画点、写文字 */
//     /* 2. 画布尺寸 */
//     int w = ui->widget0404->width();
//     int h = ui->widget0404->height();
//     int dx = w / 6;               // 水平间距
//     int midY = h / 2;
//     for (int i = 0; i < 6; ++i) {
//         points[i].setX(dx * i + dx / 2);
//         offSet = (days[i].mTempHigh.toInt() - ave) * 3;
//         points[i].setY(midY - offSet);

//         // 画出6个点
//         painter.drawEllipse(QPoint(points[i]), 3, 3);
//         // 画出当天温度
//         painter.drawText(points[i].x() - 15, points[i].y() - 10, days[i].mTempHigh + "°");
//         qDebug() << days[i].mTempHigh;
//     }
//     // 画5条线
//     for(int i = 0; i < 5; i++){
//         painter.drawLine(points[i],points[i+1]);
//     }
// }

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

// 最低温折线图绘画函数
void Widget::drawTempLineLow()
{
    // 在 widget0405 上画
    QPainter painter(ui->widget0405);
    painter.setBrush(QColor(70, 192 ,203));  // 画实心点
    painter.setRenderHint(QPainter::Antialiasing, true); // 添加抗齿，使曲线更好看
    painter.setPen(QColor(70, 192 ,203));

    // 计算平均温度
    int ave;
    int sum = 0;

    for(int i = 0; i < 6; i++){
        sum += days[i].mTempLow.toInt();
    }
    ave = sum / 6;


    QPoint points[6];  // 定义折线图的6个点
    int offSet = 0;   // 偏移值
    //int middle = ui->widget0405->height()/2;  // widget0405空间的获取中间y位置
    // 画出6个点
    // for(int i = 0; i < 6; i++){
    //     points[i].setX(mAirqList[i]->x() + mAirqList[i]->width()/2);
    //     offSet = (days[i].mTempLow.toInt() - ave) * 3; // 如果为正，说明当天温度大于平均值，那么y值应该低于middle；否则，y值高于middle
    //     points[i].setY(middle - offSet);

    //     // 画出圈圈
    //     painter.drawEllipse(QPoint(points[i]),3,3);
    //     // 画出当天温度
    //     painter.drawText(points[i].x() - 15, points[i].y() - 15, days[i].mTempLow + "°");
    // }
    /* 3. 画点、写文字 */
    /* 2. 画布尺寸 */
    int w = ui->widget0405->width();
    int h = ui->widget0405->height();
    int dx = w / 6;               // 水平间距
    int midY = h / 2;
    for (int i = 0; i < 6; ++i) {
        points[i].setX(dx * i + dx / 2);
        offSet = (days[i].mTempLow.toInt() - ave) * 3;
        points[i].setY(midY - offSet);

        // 画出6个点
        painter.drawEllipse(QPoint(points[i]), 3, 3);
        // 画出当天温度
        painter.drawText(points[i].x() - 15, points[i].y() - 10, days[i].mTempLow + "°");
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

// 用这种方法画不了，会被layout遮挡，导致展示出来没有任何效果
// void Widget::paintEvent(QPaintEvent *event)
// {
//     // 下面这样画出来没有效果，因为被layout给遮挡住了
//     QPainter painter(this);
//     painter.translate(rect().center());
//     painter.setBrush(Qt::yellow);
//     painter.drawEllipse(QPoint(0,0),60,60);
// }

// 获取城市的cityid
// QString getCityCodeFromName(QString name)
// {
//     // 打开城市id的json数据
//     QFile file(":/citycode.json");
//     file.open(QIODevice::ReadOnly);
//     QByteArray rawData = file.readAll();
//     file.close();

//     // 将Json数据转为QJsonDocument对象
//     QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
//     // 如果QJsonDocument对象是个数组
//     if(jsonDoc.isArray()){
//         // 获取Json数组
//         QJsonArray citys = jsonDoc.array();
//         // 遍历数组中每个元素(Json对象)
//         for(QJsonValue val : citys){
//             if(val.isObject()){
//                 QString cityName = val["city_name"].toString();
//                 if(cityName == name){
//                     return val["city_code"].toString();
//                 }
//             }
//         }
//     }
//     return "";
// }

// 搜索按钮的槽函数
void Widget::on_pushButton_clicked()
{
    QString cityNameFromUser = ui->lineEditCity->text();
    QString citycode = cityCodeUtils.getCityCodeFromName(cityNameFromUser);
    if(citycode != ""){
        strUrl += "&cityid=" + citycode;
        manager->get(QNetworkRequest(QUrl(strUrl)));
    }else{
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("请输入正确的城市名称");
        mes.setStyleSheet("QPushButton {color: red}");
        mes.setStandardButtons(QMessageBox::Ok);
        mes.exec();
    }
}

// 按下回车进行搜索
void Widget::on_lineEditCity_returnPressed()
{
    on_pushButton_clicked();
}


