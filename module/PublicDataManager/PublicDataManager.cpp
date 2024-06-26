﻿#include "PublicDataManager.h"

namespace module
{
	std::vector<MyFriendInfoWithFirstC>& PublicDataManager::getMyFriendInfoWithCVec()
	{
		return m_vecFriendInfoWithC;
	}

	std::vector<MyLastChatFriendInfo>& PublicDataManager::getMyLastChatFriendInfoVec()
	{
		return m_vecLastChatFriend;
	}

	std::vector<MyFriendInfoForStartGroupChat>& PublicDataManager::getFriendInfoForStartGroupChat()
	{
		if (m_vecFriendInfoForStartGroupChat.size() != m_vecFriendInfoWithC.size())
		{
			m_vecFriendInfoForStartGroupChat.clear();
			for (int i = 0; i < m_vecFriendInfoWithC.size(); ++i)
			{
				MyFriendInfoForStartGroupChat stuFriendInfo;
				stuFriendInfo.m_strId = m_vecFriendInfoWithC[i].m_strId;
				stuFriendInfo.m_strName = m_vecFriendInfoWithC[i].m_strName;
				stuFriendInfo.m_strImagePath = m_vecFriendInfoWithC[i].m_strImagePath;
				stuFriendInfo.m_bIsSelected = false;
				m_vecFriendInfoForStartGroupChat.push_back(stuFriendInfo);
			}
		}
		return m_vecFriendInfoForStartGroupChat;
	}

	void PublicDataManager::getFriendImagePathVec(std::vector<std::string>& vecId, std::vector<std::string>& strImagePath)
	{
		strImagePath.push_back(PublicDataManager::getImagePath().toStdString());
		for (int i = 0; i < vecId.size(); ++i)
		{
			strImagePath.push_back(m_vecFriendInfoWithC[m_mapUserInfo[QString::fromStdString(vecId[i])]].m_strImagePath);
		}
	}

	std::unordered_map<QString, int>& PublicDataManager::getMyUsetInfoMap()
	{
		return m_mapUserInfo;
	}

	QString PublicDataManager::getMyId() const
	{
		return m_strId;
	}

	QString PublicDataManager::getMyName() const
	{
		return m_strName;
	}

	CurrentChatWidgetUserInfo PublicDataManager::getCurrentChatWidgetUserInfo() const
	{
		return m_stuCurrentChatUserInfo;
	}

	CurrentChatWidgetState PublicDataManager::getCurrentChatWidgetState() const
	{
		return m_eCurrentChatWidgetState;
	}

	MyGroupChatInfo PublicDataManager::getCurrentGroupChatInfo() const
	{
		return m_stuGroupChatInfo;
	}

	MyFriendInfoWithFirstC PublicDataManager::getFriendInfoAcordId(const QString& id)
	{
		return m_vecFriendInfoWithC[m_mapUserInfo[id]];
	}

	void PublicDataManager::setMyFriendInfoWithC(const std::vector<MyFriendInfoWithFirstC>& vecFriendInfoWithC)
	{
		m_vecFriendInfoWithC = vecFriendInfoWithC;
	}

	void PublicDataManager::setMyLastChatInfo(const std::vector<MyLastChatFriendInfo>& vecLastChatFriend)
	{
		m_vecLastChatFriend = vecLastChatFriend;
	}

	void PublicDataManager::setUserInfo(const std::unordered_map<QString, int>& mapUserInfo)
	{
		m_mapUserInfo = mapUserInfo;
	}

	void PublicDataManager::setMyId(const QString& id)
	{
		m_strId = id;
	}

	void PublicDataManager::setMyName(const QString& name)
	{
		m_strName = name;
	}

	void PublicDataManager::setCurrentChatWidgetUserInfo(const CurrentChatWidgetUserInfo& info)
	{
		m_stuCurrentChatUserInfo = info;
	}

	void PublicDataManager::setUnreadMsg(const QString& id, int cnt)
	{
		if (m_mapUnreadMsgCnt.count(id))
		{
			m_mapUnreadMsgCnt[id] = 0;
			m_mapUnreadMsgCnt[id] += cnt;
		}
		else
		{
			m_mapUnreadMsgCnt[id] += cnt;
		}
	}

	void PublicDataManager::setIdDirPath(const QString& path)
	{
		m_strIdDirPath = path;
	}

	void PublicDataManager::setImagePath(const QString& path)
	{
		m_strImagePath = path;
	}

	void PublicDataManager::setCurrentChatWidgetState(CurrentChatWidgetState state)
	{
		m_eCurrentChatWidgetState = state;
	}

	void PublicDataManager::updateProfileImagePathOfFriendInfoVec(const QString& id, const QString& path)
	{
		m_vecFriendInfoWithC[m_mapUserInfo[id]].m_strImagePath = path.toStdString();
	}

	void PublicDataManager::clearUnreadMsg(const QString& id)
	{
		m_mapUnreadMsgCnt.erase(id);
	}

	int PublicDataManager::getUnreadMsgCnt(const QString& id) const
	{
		return m_mapUnreadMsgCnt.at(id);
	}

	QString PublicDataManager::getIdDirPath() const
	{
		return m_strIdDirPath;
	}

	QString PublicDataManager::getImagePath() const
	{
		return m_strImagePath;
	}

	void PublicDataManager::addFriendInfoWithC(const MyFriendInfoWithFirstC& friendInfo)
	{
		for (int i = 0; i < m_vecFriendInfoWithC.size(); ++i)
		{
			if (friendInfo.m_strFirstChacter <= m_vecFriendInfoWithC[i].m_strFirstChacter)
			{
				m_vecFriendInfoWithC.insert(m_vecFriendInfoWithC.begin() + i, friendInfo);
				m_mapUserInfo[friendInfo.m_strId.c_str()] = i;
				return;
			}
		}
		m_vecFriendInfoWithC.push_back(friendInfo);
		m_mapUserInfo[friendInfo.m_strId.c_str()] = m_vecFriendInfoWithC.size() - 1;
	}

	bool PublicDataManager::isIdExistInLastChatList(const QString& id) const
	{
		for (auto& listItem : m_vecLastChatFriend)
		{
			if (listItem.m_strId == id)
			{
				return true;
			}
		}
		return false;
	}

	void PublicDataManager::insertLastChatList(const MyLastChatFriendInfo& info)
	{
		m_vecLastChatFriend.push_back(info);
	}
}
