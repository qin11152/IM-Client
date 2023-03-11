#pragma once

#include "module/stdafx.h"
#include <QStyledItemDelegate>

class AddedFriendDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	AddedFriendDelegate(QObject* parent = nullptr);
	~AddedFriendDelegate();

signals:
	void signalCloseFromAddedList(const QString& id);

protected:
	/**
	 * brief���ػ��¼��������Լ���Ҫ������.
	 *
	 * \param painter������
	 * \param option������
	 * \param index��λ��
	 */
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)const override;

	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

	/**
	 * brief�����سߴ�.
	 *
	 * \param option
	 * \param index
	 * \return
	 */
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)const override;
};
