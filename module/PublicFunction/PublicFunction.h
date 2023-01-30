#pragma once

#include <QObject>
#include <string>
#include <windows.h>

namespace Base
{
    namespace PinYin
    {
        bool IsChinese(QChar qch);
        QString convertToPinYin(const QString& chinese);
    }

    //��ʱ�䰴��ָ����ʽ���Ϊ�ַ���
    std::string timeToString(const std::string& format = "%Y-%m-%d %H:%M:%S");
}

