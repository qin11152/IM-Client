#pragma once

#include "module/stdafx.h"

#include <QAbstractListModel>

/**
 * brief�������б�ģ��ʹ�õ����ݽṹ��.
 */
struct FriendListInfo
{
	QString m_strFriendId{ "" };	//����id
	QString m_strFriendName{ "" };	//��������
	QString m_strFriendImagePath{ "" };	//����ͷ��·��
};

class MyFriendListModel  : public QAbstractListModel
{
	Q_OBJECT

public:
	MyFriendListModel(QObject* parent = nullptr);
	~MyFriendListModel();

	void setData(std::vector<FriendListInfo>& friendListInfo);
	void addData(const FriendListInfo& friendListInfo);

protected:
	QVariant data(const QModelIndex& index, int role) const override;
	int rowCount(QModelIndex const& parent = QModelIndex()) const override;
	QMap<int, QVariant> itemData(const QModelIndex& index)const override;
	QHash<int, QByteArray> roleNames()const override;

private:
	std::vector<FriendListInfo> m_vecFriendListInfo;
};

Q_DECLARE_METATYPE(FriendListInfo);
