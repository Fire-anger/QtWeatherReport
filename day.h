#ifndef DAY_H
#define DAY_H

#include <QString>


class Day
{
public:
    Day();
    // 下面是每一天的天气数据
    QString mWeek;         // 星期
    QString mDate;         // 日期
    QString mCity;        // 城市
    QString mTemp;       // 当前温度
    QString mWeathType;  // 天气类型 晴
    QString mTempLow;    // 最低气温
    QString mTempHigh;   // 最高气温

    QString mTips;       // 感冒指数/提示
    QString mFx;         // 风向
    QString mFl;         // 风力
    QString MPm25;       // PM2.5
    QString mShiDu;      // 湿度
    QString mAirq;       // 空气质量

};

#endif // DAY_H
