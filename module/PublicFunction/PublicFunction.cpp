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
					//这里的Unicode_Table就是按照UNICODE每个中文对应的拼音数组;
					pinyins.append(Unicode_Table[unicode - 0x4E00]);
				}
				else
				{
					//这里不是中文，我们不做任何处理，保留;
					pinyins.append(chinese.at(i));
				}
				//pinyins.append(" ");
			}
			return pinyins;
		}
	}

	//使用put_time，按照格式要求将时间转换为字符串
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
	//根据好友头像路径vec的大小
	QImage image(50, 50, QImage::Format_ARGB32);
	switch (cnt)
	{
	case 3:
	{
		//绘制一个50×50的图片，按照vec中的路径，第一个在第一行居中，第二三个在第二行
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
		//绘制一个50*50的图片,两行两列，前两个在第一行，后两个在第二行
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
		//绘制一个50*50的图片,两行三列，前两个在第一行，后三个在第二行
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
		//绘制一个50*50的图片,两行三列，前三个在第一行，后三个在第二行
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
		//绘制一个50*50的图片,三行三列，第一个在第一行，第二到四个在第二行，最后三个在第三行
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
		//绘制一个50*50的图片,三行三列，前两个在第一行，第三到五个在第二行，最后三个在第三行
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
		//绘制一个50*50的图片,三行三列，前三个在第一行，第四到六个在第二行，最后三个在第三行
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
