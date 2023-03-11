#include "StartGroupChatWidget.h"

StartGroupChatWidget::StartGroupChatWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initUI();
	initData();
	initConnect();
}

StartGroupChatWidget::~StartGroupChatWidget()
{}

void StartGroupChatWidget::setModelData(std::vector<MyFriendInfoForStartGroupChat>&friendInfo)
{
	if (m_ptrChooseFriendModel)
	{
		m_ptrChooseFriendModel->setData(friendInfo);
	}
}

void StartGroupChatWidget::onSignalFinishClicked()
{
}

void StartGroupChatWidget::onSignalCancelClicked()
{
	if (m_ptrAddFriendModel)
	{
		m_ptrAddFriendModel->clearModel();
	}
	hide();
}

void StartGroupChatWidget::onSignalUpdateSelectedState(const QString& id, bool newState)
{
	if (m_ptrChooseFriendModel)
	{
		m_ptrChooseFriendModel->updateModelSelectedState(id);
	}
	//TODO 更新到已选择的页面中
	if (m_ptrAddFriendModel)
	{
		m_ptrAddFriendModel->onStateChanged(id, newState);
	}
}

void StartGroupChatWidget::onSignalSearchChanged(const QString& searchMsg)
{
	QString strPinYin = "";
	strPinYin = Base::PinYin::convertToPinYin(searchMsg);
	std::map<int, QVariant> tmpMap;
	tmpMap[(int)UserRoleDefine::FriendListName] = QVariant(strPinYin);
	if (m_ptrChooseFriendSortModel)
	{
		m_ptrChooseFriendSortModel->setFilterMap(tmpMap);
	}

}

void StartGroupChatWidget::onSignalAddedDeleted(const QString& id)
{
	if (m_ptrChooseFriendModel)
	{
		m_ptrChooseFriendModel->updateModelSelectedState(id);
	}
	if(m_ptrAddFriendModel)
	{
		m_ptrAddFriendModel->removeAcordId(id);
	}
}

void StartGroupChatWidget::initUI()
{
	setWindowFlags(Qt::FramelessWindowHint);
	setFixedSize({ 755,550 });

	ui.searchLineEdit->setFixedSize({ 300,40 });
	ui.searchLineEdit->setPlaceholderText(QString::fromLocal8Bit("请输入好友名称搜索"));

	ui.addedListView->setAttribute(Qt::WA_TranslucentBackground);
	ui.addedListView->setFixedSize({ 380,440 });
	ui.addedListView->setFlow(QListView::LeftToRight);
	ui.addedListView->setViewMode(QListView::IconMode);
	ui.addedListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
	ui.addedListView->setAutoScrollMargin(10);

	ui.chooseListView->setFixedWidth(340);
	ui.chooseListView->setAttribute(Qt::WA_TranslucentBackground);

	ui.finishPushButton->setFixedSize({ 130,40 });
	ui.cancelPushButton->setFixedSize({ 130,40});
}

void StartGroupChatWidget::initData()
{
	m_ptrChooseFriendModel=new ChooseFriendModel();
	m_ptrChooseFriendDelegate = new ChooseFriendDelegate();
	m_ptrChooseFriendSortModel=new ChooseFriendSortModel();
	m_ptrChooseFriendSortModel->setSourceModel(m_ptrChooseFriendModel);
	ui.chooseListView->setModel(m_ptrChooseFriendSortModel);
	ui.chooseListView->setItemDelegate(m_ptrChooseFriendDelegate);

	m_ptrAddFriendModel=new AddedFriendModel();
	m_ptrAddFriendDelegate=new AddedFriendDelegate();
	ui.addedListView->setModel(m_ptrAddFriendModel);
	ui.addedListView->setItemDelegate(m_ptrAddFriendDelegate);
}

void StartGroupChatWidget::initConnect()
{
	connect(ui.finishPushButton, &QPushButton::clicked, this, &StartGroupChatWidget::onSignalFinishClicked);
	connect(ui.cancelPushButton, &QPushButton::clicked, this, &StartGroupChatWidget::onSignalCancelClicked);
	connect(m_ptrChooseFriendDelegate,&ChooseFriendDelegate::signalClickChooseFriendBox,this,&StartGroupChatWidget::onSignalUpdateSelectedState);
	connect(ui.searchLineEdit, &QLineEdit::textChanged, this, &StartGroupChatWidget::onSignalSearchChanged);
	connect(m_ptrAddFriendDelegate, &AddedFriendDelegate::signalCloseFromAddedList, this, &StartGroupChatWidget::onSignalAddedDeleted);
}
