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

void StartGroupChatWidget::setModelData(std::vector<MyFriendInfoWithFirstC>&friendInfo)
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
	hide();
}

void StartGroupChatWidget::initUI()
{
	setWindowFlags(Qt::FramelessWindowHint);
	setFixedSize({ 755,550 });
	ui.searchLineEdit->setFixedSize({ 300,40 });
	ui.addedListView->setAttribute(Qt::WA_TranslucentBackground);
	ui.addedListView->setFixedSize({ 380,440 });
	ui.chooseListView->setFixedWidth(340);
	ui.chooseListView->setAttribute(Qt::WA_TranslucentBackground);
	ui.finishPushButton->setFixedSize({ 130,40 });
	ui.cancelPushButton->setFixedSize({ 130,40});
	ui.searchLineEdit->setPlaceholderText(QString::fromLocal8Bit("ÇëÊäÈëºÃÓÑÃû³ÆËÑË÷"));
}

void StartGroupChatWidget::initData()
{
	m_ptrChooseFriendModel=new ChooseFriendModel();
	m_ptrChooseFriendDelegate = new ChooseFriendDelegate();
	ui.chooseListView->setModel(m_ptrChooseFriendModel);
	ui.chooseListView->setItemDelegate(m_ptrChooseFriendDelegate);
}

void StartGroupChatWidget::initConnect()
{
	connect(ui.finishPushButton, &QPushButton::clicked, this, &StartGroupChatWidget::onSignalFinishClicked);
	connect(ui.cancelPushButton, &QPushButton::clicked, this, &StartGroupChatWidget::onSignalCancelClicked);
}
