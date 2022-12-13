#include "AddFriendWidget.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"

AddFriendWidget::AddFriendWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUI();
    initData();
    initConnect();
}

AddFriendWidget::~AddFriendWidget()
{
}

void AddFriendWidget::onSignalAgreeClicked(const QString& id)
{
    updateModelValidState(id, true);
    ChatWidgetManager::Instance()->onSignalAgreeAddFriend(id);
}

void AddFriendWidget::closeEvent(QCloseEvent* event)
{
    ui.accountLineEdit->clear();
    ui.verifyInfoLineEdit->clear();
}

void AddFriendWidget::setData(std::vector<AddFriendInfo>& vecFriendInfo)
{
    m_ptrModel->setData(vecFriendInfo);
}

void AddFriendWidget::updateModelValidState(const QString& id, bool validState)
{
    m_ptrModel->updateModelValidState(id, validState);
}

void AddFriendWidget::updateModelImagePath(const QString& id, const QString& path)
{
    m_ptrModel->updateModelImagePath(id, path);
}

void AddFriendWidget::initUI()
{
    ui.accountLineEdit->setPlaceholderText(QString::fromLocal8Bit("ÇëÊäÈëºÃÓÑÕËºÅ"));
    ui.verifyInfoLineEdit->setPlaceholderText(QString::fromLocal8Bit("ÇëÊäÈëÑéÖ¤ÄÚÈÝ"));

    m_notifyLabel = new NotifyLabel(this);
    setFixedSize({ 800,400 });
}

void AddFriendWidget::initConnect()
{
    connect(ui.pushButton, &QPushButton::clicked, this, &AddFriendWidget::onSignalAddBtnClicked);
    connect(m_ptrDelegate, &AddFriendDelegate::signalAgreeAdd, this, &AddFriendWidget::onSignalAgreeClicked,Qt::QueuedConnection);
}

void AddFriendWidget::initData()
{
    m_ptrModel = new AddFriendModel(ui.listView);
    m_ptrDelegate = new AddFriendDelegate();
    ui.listView->setModel(m_ptrModel);
    ui.listView->setItemDelegate(m_ptrDelegate);
}

void AddFriendWidget::onSignalAddBtnClicked()
{
    if (ui.accountLineEdit->text().isEmpty())
    {
        m_notifyLabel->setNitifyText(QString::fromLocal8Bit("ÇëÊäÈëºÃÓÑÕËºÅ").toStdString().c_str());
        m_notifyLabel->showNotify();
        return;
    }
    QString account = ui.accountLineEdit->text();
    QString verifyInfo = ui.verifyInfoLineEdit->text();
    ChatWidgetManager::Instance()->onSignalRequestAddFriend(account, verifyInfo);
}

