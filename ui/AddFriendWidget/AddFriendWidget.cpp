#include "AddFriendWidget.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"

AddFriendWidget::AddFriendWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUI();
    initConnect();
    initData();
}

AddFriendWidget::~AddFriendWidget()
{
}

void AddFriendWidget::closeEvent(QCloseEvent* event)
{
    ui.accountLineEdit->clear();
    ui.verifyInfoLineEdit->clear();
}

void AddFriendWidget::initUI()
{
    ui.accountLineEdit->setPlaceholderText(QString::fromLocal8Bit("请输入好友账号"));
    ui.verifyInfoLineEdit->setPlaceholderText(QString::fromLocal8Bit("请输入验证内容"));

    m_notifyLabel = new NotifyLabel(this);
    setFixedSize({ 800,400 });
}

void AddFriendWidget::initConnect()
{
    connect(ui.pushButton, &QPushButton::clicked, this, &AddFriendWidget::onSignalAddBtnClicked);
}

void AddFriendWidget::initData()
{
    std::vector<AddFriendInfo> vecAddFriendInfo;
    for (int i = 0; i < 2; ++i)
    {
        auto tmpinfo = AddFriendInfo();
        tmpinfo.isValid = false;
        tmpinfo.m_strFriendId = QString::number(i);
        tmpinfo.m_strProfileImagePath = "D:/q.jpg";
        tmpinfo.m_strVerifyInfo = "dakjshdkjas";
        vecAddFriendInfo.push_back(tmpinfo);
    }
    m_ptrModel = new AddFriendModel(ui.listView);
    m_ptrModel->setData(vecAddFriendInfo);
    m_ptrDelegate = new AddFriendDelegate();
    ui.listView->setModel(m_ptrModel);
    ui.listView->setItemDelegate(m_ptrDelegate);
}

void AddFriendWidget::onSignalAddBtnClicked()
{
    if (ui.accountLineEdit->text().isEmpty())
    {
        m_notifyLabel->setNitifyText(QString::fromLocal8Bit("请输入好友账号").toStdString().c_str());
        m_notifyLabel->showNotify();
        return;
    }
    QString account = ui.accountLineEdit->text();
    QString verifyInfo = ui.verifyInfoLineEdit->text();
    ChatWidgetManager::Instance()->onSignalRequestAddFriend(account, verifyInfo);
}

