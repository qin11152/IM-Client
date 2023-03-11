#include "ChooseFriendDelegate.h"

ChooseFriendDelegate::ChooseFriendDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

ChooseFriendDelegate::~ChooseFriendDelegate()
{}

void ChooseFriendDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QString strNmae = index.data((int)UserRoleDefine::FriendListName).toString();
	QString strImagePath = index.data((int)UserRoleDefine::FriendListImagePath).toString();
	auto isSelected=index.data((int)UserRoleDefine::ChooseFriendIsSelected).toBool();

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

	//如果model中为选中状态
	if (isSelected)
	{
		button.state |= QStyle::State_On;
	}
	else
	{
		button.state |= QStyle::State_Off;
	}
	QApplication::style()->drawControl(QStyle::CE_CheckBox, &button, painter);
}

bool ChooseFriendDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	//auto pEvent = reinterpret_cast<QMouseEvent*>(event);
	if (QEvent::MouseButtonPress==event->type())
	{
		QRect CheckBoxRect(option.rect.x() + 25, option.rect.y() + 20, 20, 20);
		auto pEvent = reinterpret_cast<QMouseEvent*>(event);
		if (CheckBoxRect.contains(pEvent->pos()))
		{
			QString id = index.data(int(UserRoleDefine::FriendListId)).toString();
			emit signalClickChooseFriendBox(id,!index.data((int)UserRoleDefine::ChooseFriendIsSelected).toBool());
			return true;
		}
	}
	return false;
}

QSize ChooseFriendDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize({ 340,60 });
}
