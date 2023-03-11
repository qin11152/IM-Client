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
	 * brief:����Ⱥ�Ľ����еĺ���model��������.
	 * 
	 * \param friendInfo:��ǰ�ĺ����б�
	 */
	void setData(std::vector<MyFriendInfoForStartGroupChat>& friendInfo);

	/**
	 * brief:����model��ѡ��״̬.
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

	std::unordered_map<QString, int> m_mapItemPosAcordId;	//����id����vec�е�λ�ã��ռ任ʱ��
};
