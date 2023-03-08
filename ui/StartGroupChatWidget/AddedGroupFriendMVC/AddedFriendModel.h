#pragma once

#include <QObject>

class AddedFriendModel  : public QObject
{
	Q_OBJECT

public:
	AddedFriendModel(QObject *parent);
	~AddedFriendModel();
};
