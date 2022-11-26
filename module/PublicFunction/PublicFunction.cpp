#include "PublicFunction.h"
#include "PinYInTable.h"

namespace Base
{
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
                //�����Unicode_Table���ǰ���UNICODEÿ�����Ķ�Ӧ��ƴ������;
                pinyins.append(Unicode_Table[unicode - 0x4E00]);
            }
            else
            {
                //���ﲻ�����ģ����ǲ����κδ�������;
                pinyins.append(chinese.at(i));
            }
            pinyins.append(" ");
        }
        return pinyins;
    }

    //ʹ��put_time�����ո�ʽҪ��ʱ��ת��Ϊ�ַ���
    std::string timeToString(const std::string& format)
    {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::ostringstream oss;
        oss << std::put_time(&tm, format.c_str());
        return oss.str();
    }
}
