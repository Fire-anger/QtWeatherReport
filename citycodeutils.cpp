#include "citycodeutils.h"

#include <qfile.h>
#include <qjsonarray.h>
#include <qjsondocument.h>

CityCodeUtils::CityCodeUtils() {}

QString CityCodeUtils::getCityCodeFromName(QString cityName)
{
    if(cityMap.isEmpty()){
        initCityMap();
    }
    QMap<QString, QString>::iterator it = cityMap.find(cityName);
    if(it == cityMap.end()){
        it = cityMap.find(cityName + "市");
        if(it == cityMap.end()){
            it = cityMap.find(cityName + "县");
        }
        if(it == cityMap.end()){
            it = cityMap.find(cityName + "区");
        }
        if(it == cityMap.end()){
            return "";
        }
    }
    return it.value();
}

// 打开关闭文件
void CityCodeUtils::initCityMap()
{
    // 打开城市id的json数据
    QFile file(":/citycode.json");
    file.open(QIODevice::ReadOnly);
    QByteArray rawData = file.readAll();
    file.close();

    // 将Json数据转为QJsonDocument对象
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    // 如果QJsonDocument对象是个数组
    if(jsonDoc.isArray()){
        // 获取Json数组
        QJsonArray citys = jsonDoc.array();
        // 遍历数组中每个元素(Json对象)
        for(const QJsonValue &val : std::as_const(citys)){
            if(val.isObject()){
                QString cityName = val["city_name"].toString();
                QString cityCode = val["city_code"].toString();
                // 向Map中添加元素
                cityMap.insert(cityName, cityCode);
            }
        }
    }
}




