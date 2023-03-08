#include "ChooseFriendDelegate.h"

#include <QPainter>
#include <QMouseEvent>
#include <QApplication>

ChooseFriendDelegate::ChooseFriendDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

ChooseFriendDelegate::~ChooseFriendDelegate()
{}

void ChooseFriendDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QString strNmae = index.data((int)UserRoleDefine::FriendListName).toString();
	QString strImagePath = index.data((int)UserRoleDefine::FriendListImagePath).toString();

	if (strImagePath.startsWith("qrc:"))
	{
		strImagePath= strImagePath.mid(3);
	}

	QRect imageRect(option.rect.x() + 60, option.rect.y() + 10, 36, 36);
	QRect NameRect(option.rect.x() + 110, option.rect.y() + 20, 200, 20);
	QRect checkBoxRect(option.rect.x() + 25, option.rect.y() + 20, 20, 20);

	auto displayOption = option;
	displayOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;

	painter->drawPixmap(imageRect, QPixmap(strImagePath));
	painter->drawText(NameRect, strNmae);

	QStyleOptionButton button;
	button.rect = checkBoxRect;
	button.state |= QStyle::State_Enabled;
	button.state |= QStyle::State_On;
	QApplication::style()->drawControl(QStyle::CE_CheckBox, &button, painter);
}

bool ChooseFriendDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	auto pEvent = reinterpret_cast<QMouseEvent*>(event);
	if (pEvent->button() == Qt::LeftButton)
	{
		if (option.rect.contains(pEvent->pos()))
		{
			QString id = index.data(int(UserRoleDefine::FriendListId)).toString();
			//emit signalChooseOneFriend(id);
			return true;
		}
	}
	return false;
}

QSize ChooseFriendDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize({ 340,60 });
}
