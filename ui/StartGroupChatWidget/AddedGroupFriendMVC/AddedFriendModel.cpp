#include "AddedFriendModel.h"

AddedFriendModel::AddedFriendModel(QObject *parent)
	: QAbstractListModel(parent)
{
	beginResetModel();
	endResetModel();
}

AddedFriendModel::~AddedFriendModel()
{}

void AddedFriendModel::onStateChanged(const QString & id, bool newState)
{
	//如果为true，就是选中了，就添加一下
	if (newState)
	{
		auto friendInfo = PublicDataManager::get_mutable_instance().getFriendInfoAcordId(id);
		beginInsertRows(QModelIndex(), m_vecAddedFriend.size(), m_vecAddedFriend.size());
		m_vecAddedFriend.push_back(friendInfo);	
		endInsertRows();
	}
	else
	{

		for (int i = 0; i < m_vecAddedFriend.size(); ++i)
		{
			if (m_vecAddedFriend[i].m_strId.c_str() == id);
			{
				qDebug() << i;
				beginRemoveRows(QModelIndex(), i, i);
				m_vecAddedFriend.erase(m_vecAddedFriend.begin() + i);
				endRemoveRows();
				break;
			}
		}
		
	}
}

void AddedFriendModel::clearModel()
{
	beginResetModel();
	m_vecAddedFriend.clear();
	endResetModel();
}

void AddedFriendModel::removeAcordId(const QString& id)
{
	for (int i = 0; i < m_vecAddedFriend.size(); ++i)
	{
		if (QString::fromStdString(m_vecAddedFriend[i].m_strId) == id)
		{
			beginRemoveRows(QModelIndex(), i, i);
			m_vecAddedFriend.erase(m_vecAddedFriend.begin() + i);
			endRemoveRows();
			break;
		}
	}
}

std::vector<std::string> AddedFriendModel::getAllId()
{
	std::vector<std::string> allId;
	for (auto& item : m_vecAddedFriend)
	{
		allId.push_back(item.m_strId);
	}
	return allId;
}

std::string AddedFriendModel::getGroupName()
{
	std::string name = "";
	for (int i = 0; name.size() < kMaxGroupName && i < m_vecAddedFriend.size(); ++i)
	{
		name += m_vecAddedFriend[i].m_strName;
	}
	return name;
}

QVariant AddedFriendModel::data(QModelIndex const& index, int role) const
{
	if (!index.isValid() || index.row() < 0 || index.row() >= m_vecAddedFriend.size())
	{
		return QVariant();
	}
	switch (role)
	{
		case int(UserRoleDefine::FriendListId):
		{
			return m_vecAddedFriend[index.row()].m_strId.c_str();
		}
			break;
		case int(UserRoleDefine::FriendListName):
		{
			return m_vecAddedFriend[index.row()].m_strName.c_str();
		}
			break;
		case int(UserRoleDefine::FriendListImagePath) :
		{
			return m_vecAddedFriend[index.row()].m_strImagePath.c_str();
		}
			break;
		default:
		{
			return QVariant();
		}
			break;
	}
	return QVariant();
}

int AddedFriendModel::rowCount(QModelIndex const& parent) const
{
	return m_vecAddedFriend.size();
}

QMap<int, QVariant> AddedFriendModel::itemData(const QModelIndex& index) const
{
	QMap<int, QVariant> roles;

	int iDataIndex = index.row();

	auto tmpInfo = m_vecAddedFriend[iDataIndex];

	roles.insert((int)UserRoleDefine::FriendListId, tmpInfo.m_strId.c_str());
	roles.insert((int)UserRoleDefine::FriendListName, tmpInfo.m_strName.c_str());
	roles.insert((int)UserRoleDefine::FriendListImagePath, tmpInfo.m_strImagePath.c_str());

	return roles;
}

QHash<int, QByteArray> AddedFriendModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[(int)UserRoleDefine::FriendListName] = "name";
	roles[(int)UserRoleDefine::FriendListImagePath] = "imagePath";
	roles[(int)UserRoleDefine::FriendListId] = "idx";
	return roles;
}
