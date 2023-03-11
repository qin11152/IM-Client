#include "AddedFriendDelegate.h"

AddedFriendDelegate::AddedFriendDelegate(QObject *parent)
	: QStyledItemDelegate(parent)
{}

AddedFriendDelegate::~AddedFriendDelegate()
{}

void AddedFriendDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
	QString strNmae = index.data((int)UserRoleDefine::FriendListName).toString();
	QString strImagePath = index.data((int)UserRoleDefine::FriendListImagePath).toString();

	if (strImagePath.startsWith("qrc:"))
	{
		strImagePath = strImagePath.mid(3);
	}

	QRect imageRect(option.rect.x() + 35, option.rect.y() + 10, 50, 50);
	QRect NameRect(option.rect.x() + 20, option.rect.y() + 70, 80, 20);
	QRect closeRect(option.rect.x() + 79, option.rect.y() + 1, 15, 15);

	auto displayOption = option;
	displayOption.displayAlignment = Qt::AlignLeft | Qt::AlignVCenter;

	painter->drawPixmap(imageRect, QPixmap(strImagePath));
	painter->drawPixmap(closeRect, QPixmap(":/LogInWidget/image/close.png"));

	QFont font("msyh", 10);
	painter->setFont(font);
	QFontMetrics metrics(font);
	auto eidleName = metrics.elidedText(strNmae, Qt::ElideRight, 80);
	painter->drawText(NameRect, Qt::AlignCenter, eidleName);

}

bool AddedFriendDelegate::editorEvent(QEvent* event, QAbstractItemModel* model, const QStyleOptionViewItem& option, const QModelIndex& index)
{
	QRect closeRect{ option.rect.x() + 79, option.rect.y() + 1, 15, 15 };
	if (QEvent::MouseButtonPress == event->type())
	{
		auto mouseEvent = reinterpret_cast<QMouseEvent*>(event);
		if (closeRect.contains(mouseEvent->pos()))
		{
			auto id= index.data((int)UserRoleDefine::FriendListId).toString();
			emit signalCloseFromAddedList(id);
		}
	}
	return false;
}

QSize AddedFriendDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	return QSize{ 110,100 };
}
