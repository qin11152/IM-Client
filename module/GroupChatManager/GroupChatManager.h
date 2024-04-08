#pragma once

#include "boost/serialization/singleton.hpp"

#include <QObject>

namespace module
{
	class GroupChatManager : public QObject, public boost::serialization::singleton<GroupChatManager>, public boost::noncopyable
	{
		Q_OBJECT

	public:
		GroupChatManager(QObject* parent = nullptr);
		~GroupChatManager();
	};
}
