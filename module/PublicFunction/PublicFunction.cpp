#include "PublicFunction.h"
#include "PinYInTable.h"

bool IsChinese(QChar qch)
{
    ushort unicode = qch.unicode();
    if (unicode >= 0x4E00 && unicode <= 0x9FA5)
    {
        return true;
    }
    return false;
}

QString convertToPinYin(const QString& chinese)
{
    QString pinyins;
    for (int i = 0; i < chinese.length(); ++i)
    {
        int unicode = QString::number(chinese.at(i).unicode(), 10).toInt();
        if (unicode >= 0x4E00 && unicode <= 0x9FA5)
        {
            //这里的Unicode_Table就是按照UNICODE每个中文对应的拼音数组;
            pinyins.append(Unicode_Table[unicode - 0x4E00]);
        }
        else
        {
            //这里不是中文，我们不做任何处理，保留;
            pinyins.append(chinese.at(i));
        }
        pinyins.append(" ");
    }
    return pinyins;
}
