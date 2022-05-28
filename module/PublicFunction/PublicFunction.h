#pragma once

#include <QObject>
#include <string>
#include <windows.h>

bool IsChinese(QChar qch);
QString convertToPinYin(const QString& chinese);
