#pragma once

#include <QObject>

class AddedFriendDelegate  : public QObject
{
	Q_OBJECT

public:
	AddedFriendDelegate(QObject *parent);
	~AddedFriendDelegate();
};
