#pragma once

#include <QObject>
#include <string>
#include <windows.h>

namespace Base
{
    bool IsChinese(QChar qch);
    QString convertToPinYin(const QString& chinese);

    //将时间按照指定格式输出为字符串
    std::string timeToString(const std::string& format = "%Y-%m-%d %H:%M:%S");
}

