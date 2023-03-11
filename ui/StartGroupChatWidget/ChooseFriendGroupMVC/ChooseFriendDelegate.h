#pragma once
#include "module/stdafx.h"
#include <QStyledItemDelegate>

class ChooseFriendDelegate  : public QStyledItemDelegate
{
	Q_OBJECT

public:
	ChooseFriendDelegate(QObject* parent = nullptr);
	~ChooseFriendDelegate();

signals:
	/**
	 * brief:ѡ��ҳ�����󷢳����ź�.
	 * [param] id:�����id
	 * [newState]:���֮���״̬���µ�״̬
	 */
	void signalClickChooseFriendBox(const QString& id,bool newState);

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
