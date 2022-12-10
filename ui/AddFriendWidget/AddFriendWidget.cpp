#include "AddFriendWidget.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"

AddFriendWidget::AddFriendWidget(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    initUI();
    initConnect();
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
    ui.accountLineEdit->setPlaceholderText(QString::fromLocal8Bit("ÇëÊäÈëºÃÓÑÕËºÅ"));
    ui.verifyInfoLineEdit->setPlaceholderText(QString::fromLocal8Bit("ÇëÊäÈëÑéÖ¤ÄÚÈÝ"));

    m_notifyLabel = new NotifyLabel(this);
}

void AddFriendWidget::initConnect()
{
    connect(ui.pushButton, &QPushButton::clicked, this, &AddFriendWidget::onSignalAddBtnClicked);
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

