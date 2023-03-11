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
	 * brief:ѡ���Ǳ�״̬�仯�󴫵ݵ����.
	 * 
	 * \param id:�仯��id
	 * \param newState:�µ�ѡ��״̬��true��false
	 */
	void onStateChanged(const QString& id,bool newState);

	/**
	 * brief:���model.
	 * 
	 */
	void clearModel();

	/**
	 * brief:����id��model���Ƴ�ĳ��.
	 * 
	 * \param id:Ҫ�Ƴ���id
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
