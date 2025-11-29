#ifndef CITYCODEUTILS_H
#define CITYCODEUTILS_H

#include <QMap>


class CityCodeUtils
{
public:
    CityCodeUtils();
    QMap<QString,QString> cityMap;
    QString getCityCodeFromName(QString cityName);
    void initCityMap();
};

#endif // CITYCODEUTILS_H
