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
	 * brief:选中页面点击后发出的信号.
	 * [param] id:点击的id
	 * [newState]:点击之后的状态，新的状态
	 */
	void signalClickChooseFriendBox(const QString& id,bool newState);

protected:
	/**
	 * brief：重绘事件，绘制自己需要的内容.
	 *
	 * \param painter：画笔
	 * \param option：区域
	 * \param index：位置
	 */
	void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index)const override;

	bool editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index) override;

	/**
	 * brief：返回尺寸.
	 *
	 * \param option
	 * \param index
	 * \return
	 */
	QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index)const override;
};
