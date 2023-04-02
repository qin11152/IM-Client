#include "PublicFunction.h"
#include "PinYInTable.h"
#include <QDir>
#include <QDateTime>

namespace Base
{
	namespace PinYin
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
				//pinyins.append(" ");
			}
			return pinyins;
		}
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

QImage Base::image::generateGridImage(int cnt, std::vector<std::string>& vecFriendImagePath)
{
	//���ݺ���ͷ��·��vec�Ĵ�С
	QImage image(50, 50, QImage::Format_ARGB32);
	switch (cnt)
	{
	case 3:
	{
		//����һ��50��50��ͼƬ������vec�е�·������һ���ڵ�һ�о��У��ڶ������ڵڶ���
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 12, 0, 25, 25 }, QPixmap(vecFriendImagePath[0].c_str()));

		painter.drawPixmap({0,25,25,25}, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({25,25,25,25}, QPixmap(vecFriendImagePath[2].c_str()));
	}
	break;
	case 4:
	{
		//����һ��50*50��ͼƬ,�������У�ǰ�����ڵ�һ�У��������ڵڶ���
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 0, 0, 25, 25 }, QPixmap(vecFriendImagePath[0].c_str()));
		painter.drawPixmap({ 25, 0, 25, 25 }, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({ 0, 25, 25, 25 }, QPixmap(vecFriendImagePath[2].c_str()));
		painter.drawPixmap({ 25, 25, 25, 25 }, QPixmap(vecFriendImagePath[3].c_str()));
	}
	break;
	case 5:
	{
		//����һ��50*50��ͼƬ,�������У�ǰ�����ڵ�һ�У��������ڵڶ���
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 8, 8, 16, 16 }, QPixmap(vecFriendImagePath[0].c_str()));
		painter.drawPixmap({ 24, 8, 16, 16 }, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({ 0,24,16,16 }, QPixmap(vecFriendImagePath[2].c_str()));
		painter.drawPixmap({ 16,24,16,16 },QPixmap(vecFriendImagePath[3].c_str()));
		painter.drawPixmap({ 32,24,16,16 },QPixmap(vecFriendImagePath[4].c_str()));
	}
	break;
	case 6:
	{
		//����һ��50*50��ͼƬ,�������У�ǰ�����ڵ�һ�У��������ڵڶ���
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 0, 8, 16, 16 }, QPixmap(vecFriendImagePath[0].c_str()));
		painter.drawPixmap({ 16, 8, 16, 16 }, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({ 32, 8, 16, 16 }, QPixmap(vecFriendImagePath[2].c_str()));
		painter.drawPixmap({ 8, 24, 16, 16 }, QPixmap(vecFriendImagePath[3].c_str()));
		painter.drawPixmap({ 24, 24, 16, 16 }, QPixmap(vecFriendImagePath[4].c_str()));
		painter.drawPixmap({ 40, 24, 16, 16 }, QPixmap(vecFriendImagePath[5].c_str()));
	}
	break;
	case 7:
	{
		//����һ��50*50��ͼƬ,�������У���һ���ڵ�һ�У��ڶ����ĸ��ڵڶ��У���������ڵ�����
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 16, 0, 16, 16 }, QPixmap(vecFriendImagePath[0].c_str()));
		painter.drawPixmap({ 0, 16, 16, 16 }, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({ 16, 16, 16, 16 }, QPixmap(vecFriendImagePath[2].c_str()));
		painter.drawPixmap({ 32, 16, 16, 16 }, QPixmap(vecFriendImagePath[3].c_str()));
		painter.drawPixmap({ 0, 32, 16, 16 }, QPixmap(vecFriendImagePath[4].c_str()));
		painter.drawPixmap({ 16, 32, 16, 16 }, QPixmap(vecFriendImagePath[5].c_str()));
		painter.drawPixmap({ 32, 32, 16, 16 }, QPixmap(vecFriendImagePath[6].c_str()));
	}
	break;
	case 8:
	{
		//����һ��50*50��ͼƬ,�������У�ǰ�����ڵ�һ�У�����������ڵڶ��У���������ڵ�����
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 8, 0, 16, 16 }, QPixmap(vecFriendImagePath[0].c_str()));
		painter.drawPixmap({ 24, 0, 16, 16 }, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({ 0, 16, 16, 16 }, QPixmap(vecFriendImagePath[2].c_str()));
		painter.drawPixmap({ 16, 16, 16, 16 }, QPixmap(vecFriendImagePath[3].c_str()));
		painter.drawPixmap({ 32, 16, 16, 16 }, QPixmap(vecFriendImagePath[4].c_str()));
		painter.drawPixmap({ 0, 32, 16, 16 }, QPixmap(vecFriendImagePath[5].c_str()));
		painter.drawPixmap({ 16, 32, 16, 16 }, QPixmap(vecFriendImagePath[6].c_str()));
		painter.drawPixmap({ 32, 32, 16, 16 }, QPixmap(vecFriendImagePath[7].c_str()));
	}
	break;
	case 9:
	{
		//����һ��50*50��ͼƬ,�������У�ǰ�����ڵ�һ�У����ĵ������ڵڶ��У���������ڵ�����
		image.fill(Qt::transparent);
		QPainter painter(&image);
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
		painter.drawPixmap({ 0, 0, 16, 16 }, QPixmap(vecFriendImagePath[0].c_str()));
		painter.drawPixmap({ 16, 0, 16, 16 }, QPixmap(vecFriendImagePath[1].c_str()));
		painter.drawPixmap({ 32, 0, 16, 16 }, QPixmap(vecFriendImagePath[2].c_str()));
		painter.drawPixmap({ 0, 16, 16, 16 }, QPixmap(vecFriendImagePath[3].c_str()));
		painter.drawPixmap({ 16, 16, 16, 16 }, QPixmap(vecFriendImagePath[4].c_str()));
		painter.drawPixmap({ 32, 16, 16, 16 }, QPixmap(vecFriendImagePath[5].c_str()));
		painter.drawPixmap({ 0, 32, 16, 16 }, QPixmap(vecFriendImagePath[6].c_str()));
		painter.drawPixmap({ 16, 32, 16, 16 }, QPixmap(vecFriendImagePath[7].c_str()));
		painter.drawPixmap({ 32, 32, 16, 16 }, QPixmap(vecFriendImagePath[8].c_str()));
	}
	break;
	default:
		break;
	}
	return image;
}
