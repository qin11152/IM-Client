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
         * brief：绘制九宫格样式的群头像.
         * 
         * int cnt：群成员数量
         */
        QImage generateGridImage(int cnt, std::vector<std::string>& vecFriendImagePath);
    }

    //将时间按照指定格式输出为字符串
    std::string timeToString(const std::string& format = "%Y-%m-%d %H:%M:%S");
}

