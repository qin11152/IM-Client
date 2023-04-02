#pragma once

#include "module/stdafx.h"
#include "module/ThreadPool/ThreadPool.h"

namespace Base
{
    namespace PinYin
    {
        bool IsChinese(QChar qch);
        QString convertToPinYin(const QString& chinese);
    }

    namespace image
    {
        /**
         * brief�����ƾŹ�����ʽ��Ⱥͷ��.
         * 
         * int cnt��Ⱥ��Ա����
         */
        QImage generateGridImage(int cnt, std::vector<std::string>& vecFriendImagePath);
    }

    //��ʱ�䰴��ָ����ʽ���Ϊ�ַ���
    std::string timeToString(const std::string& format = "%Y-%m-%d %H:%M:%S");
}

