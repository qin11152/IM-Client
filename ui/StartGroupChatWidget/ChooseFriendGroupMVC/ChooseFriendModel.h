#pragma once

#include "module/stdafx.h"

#include <QAbstractListModel>

class ChooseFriendModel  : public QAbstractListModel
{
	Q_OBJECT

public:
	ChooseFriendModel(QObject* parent = nullptr);
	~ChooseFriendModel();

	/**
	 * brief:向发起群聊界面中的好友model设置数据.
	 * 
	 * \param friendInfo:当前的好友列表
	 */
	void setData(std::vector<MyFriendInfoForStartGroupChat>& friendInfo);

	/**
	 * brief:更新model中选择状态.
	 * 
	 * \param id
	 */
	void updateModelSelectedState(const QString& id);

protected:
	QVariant data(QModelIndex const& index, int role) const override;
	int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	QMap<int, QVariant> itemData(const QModelIndex& index)const override;
	QHash<int, QByteArray> roleNames()const override;

private:
	std::vector<MyFriendInfoForStartGroupChat> m_vecFriendInfo;

	std::unordered_map<QString, int> m_mapItemPosAcordId;	//根据id查找vec中的位置，空间换时间
};
