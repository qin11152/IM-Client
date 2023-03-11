#pragma once

#include "module/stdafx.h"

#include <QAbstractListModel>

class AddedFriendModel : public QAbstractListModel
{
	Q_OBJECT

public:
	AddedFriendModel(QObject* parent = nullptr);
	~AddedFriendModel();

	/**
	 * brief:选择那边状态变化后传递到这边.
	 * 
	 * \param id:变化的id
	 * \param newState:新的选中状态，true或false
	 */
	void onStateChanged(const QString& id,bool newState);

	/**
	 * brief:清空model.
	 * 
	 */
	void clearModel();

	/**
	 * brief:根据id从model中移除某行.
	 * 
	 * \param id:要移除的id
	 */
	void removeAcordId(const QString& id);

protected:
	QVariant data(QModelIndex const& index, int role) const override;
	int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	QMap<int, QVariant> itemData(const QModelIndex& index)const override;
	QHash<int, QByteArray> roleNames()const override;

private:
	std::vector<MyFriendInfoWithFirstC> m_vecAddedFriend;
};
