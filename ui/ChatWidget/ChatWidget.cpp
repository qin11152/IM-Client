#include "../../ui/ChatWidget/ChatWidget.h"
#include "MytextEdit/MyTextEdit.h"
#include "TCPConnect/TCPConnect.h"
#include "MyLineEdit/MyLineEdit.h"
#include "DataBaseDelegate/DataBaseDelegate.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"
#include "protocol/ChatMessageJsonData/SingleChatMessageJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "ui_ChatWidget.h"
#include <QPushButton>
#include <QDebug>
#include <ctime>

const QString ChatRecordTable = "chatrecord";
static bool optMultipleSample = false;
static bool optCoreProfile = false;

ChatWidget::ChatWidget(QString id, QWidget* parent)
    : QWidget(parent),
    m_strUserId(id)
{
    ui = new Ui::ChatWidget();
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    ChatWidgetManager::Instance()->setUserId(m_strUserId);
    DataBaseDelegate::Instance()->SetUserId(m_strUserId);
    initData();
    initUi();
    ChatWidgetManager::Instance()->setQMLRootPtr(m_ptrAddFriendQMLRoot, m_ptrFriendListQMLRoot, m_ptrLastChatQMLRoot);
    initConnect();
}

bool ChatWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_ptrTrayIcon)
    {
        if (event->type() == QMouseEvent::HoverEnter)
        {
            //printf("enter\n");
        }
        if (event->type() == QMouseEvent::HoverLeave)
        {
            //printf("leave\n");
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ChatWidget::onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo)
{
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "insertElementToModel", Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strName)), Q_ARG(QVariant, ""), Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strId)));
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "initColor", Qt::DirectConnection);
    initChatMessageWidAcordId(QString::fromStdString(friendInfo.m_strId));
}

void ChatWidget::onSignalTextEditIsFocus(bool isFocus)
{
    if (isFocus)
    {
        QPalette pal = ui->widget_3->palette();
        pal.setColor(QPalette::Window, Qt::white);
        ui->widget_3->setPalette(pal);
    }
    else
    {
        QPalette pal = ui->widget_3->palette();
        pal.setColor(QPalette::Window, QColor(238, 238, 238));
        ui->widget_3->setPalette(pal);
    }
}

//点击发送消息后的处理
void ChatWidget::onSignalSendMessage()
{
    if (ui->textEdit->toPlainText().isEmpty())
    {
        ui->label->setVisible(true);
        if (m_ptrNullMessageTimer == nullptr)
        {
            m_ptrNullMessageTimer = new QTimer();
            m_ptrNullMessageTimer->setInterval(2000);
            m_ptrNullMessageTimer->start();
            connect(m_ptrNullMessageTimer, &QTimer::timeout, [=]() 
                {
                    m_ptrNullMessageTimer->stop();
                    ui->label->setVisible(false);
                });
        }
        else
        {
            if (m_ptrNullMessageTimer->isActive())
            {
                return;
            }
            m_ptrNullMessageTimer->start();
        }
    }
    else
    {
        //获取输入框中的文本
        QString lineEditMessage = ui->textEdit->toPlainText();

        //先获取到当前的界面
        auto tmpWid = static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->currentWidget());
        //获取界面对应的id
        QString id = tmpWid->GetUserId();

        //获取时间
        time_t now = time(NULL);
        tm* t = localtime(&now);
        std::stringstream ss;
        ss << t->tm_year + 1900 << "." << t->tm_mon + 1 << "." <<
            t->tm_mday << ".." << t->tm_hour << "." << t->tm_min << "." << t->tm_sec;

        //通过网络将信息发送出去
        SingleChatMessageJsonData singleChatData;
        singleChatData.m_strSendUserId = m_strUserId.toStdString();
        singleChatData.m_strRecvUserId = (id).toStdString();
        singleChatData.m_strMessage = lineEditMessage.toStdString();
        singleChatData.m_strTime = ss.str();
        std::string sendMessage = singleChatData.generateJson();
        //printf("%s\n",sendMessage.c_str());
        TCPConnect::Instance()->sendMessage(sendMessage);

        auto tablename = "chatrecord" + atoi(singleChatData.m_strRecvUserId.c_str());
        //查看数据库中这个表是否存在
        if (DataBaseDelegate::Instance()->isTableExist(tablename))
        {
            //添加到数据库
            DataBaseDelegate::Instance()->insertChatRecoed(tmpWid->getTotalRecordCount(), singleChatData.m_strRecvUserId.c_str(), QString::fromStdString(singleChatData.m_strMessage), QString::fromStdString(singleChatData.m_strTime), true, "QQQ");
        }
        else
        {
            //没有就创建这个表
            DataBaseDelegate::Instance()->createUserChatTable(QString::fromStdString(singleChatData.m_strRecvUserId));
            //添加到数据库
            DataBaseDelegate::Instance()->insertChatRecoed(tmpWid->getTotalRecordCount(), singleChatData.m_strRecvUserId.c_str(), QString::fromStdString(singleChatData.m_strMessage), QString::fromStdString(singleChatData.m_strTime), true, "QQQ");
        }

        //更新到界面中
        QMetaObject::invokeMethod(tmpWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, tmpWid->getUserName()), Q_ARG(QVariant, lineEditMessage), Q_ARG(QVariant, true), Q_ARG(QVariant, tmpWid->GetInitial()), Q_ARG(QVariant, id));
        //把消息更新到界面中
        ui->textEdit->clear();
        tmpWid->addRecordCount(1);
    }
}

void ChatWidget::onSignalFriendListClicked(QString strId)
{
    if (ui->chatStackedWidget->isWidCreate(strId.toInt()))
    {
        //TODO打开对应的聊天记录页面
        ui->chatStackedWidget->SwitchToChatPage(strId.toInt());
    }
}

void ChatWidget::onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason)
{
    //单击托盘图标时
    if (QSystemTrayIcon::ActivationReason::Trigger == reason)
    {
        //聊天界面显示出来
        showNormal();
    }
}

void ChatWidget::onSignalSingleChatMessage(const QString& chatMessage)
{
    SingleChatMessageJsonData singleChatData(chatMessage.toStdString());
    //TODO 存储在数据库中
    auto tablename = "chatrecord" + atoi(singleChatData.m_strSendUserId.c_str());
    auto tmpWid = static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->getWidAcord2Id(atoi(singleChatData.m_strSendUserId.c_str())));
    if (DataBaseDelegate::Instance()->isTableExist(tablename))
    {
        //添加到数据库
        DataBaseDelegate::Instance()->insertChatRecoed(tmpWid->getTotalRecordCount(), singleChatData.m_strSendUserId.c_str(), QString::fromStdString(singleChatData.m_strMessage), QString::fromStdString(singleChatData.m_strTime), false, "QQQ");
    }
    else
    {
        //没有就创建这个表
        DataBaseDelegate::Instance()->createUserChatTable(QString::fromStdString(singleChatData.m_strSendUserId));
        //添加到数据库
        DataBaseDelegate::Instance()->insertChatRecoed(tmpWid->getTotalRecordCount(), singleChatData.m_strSendUserId.c_str(), QString::fromStdString(singleChatData.m_strMessage), QString::fromStdString(singleChatData.m_strTime), false, "QQQ");
    }
    QMetaObject::invokeMethod(tmpWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, tmpWid->getUserName()), Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strMessage)), Q_ARG(QVariant, false), Q_ARG(QVariant, tmpWid->GetInitial()), Q_ARG(QVariant, atoi(singleChatData.m_strRecvUserId.c_str())));
    tmpWid->addRecordCount(1);
}

void ChatWidget::onSignalSearchTextLoseFocus(bool isFocus)
{
    if (isFocus)
    {
        if (ui->lineEdit->text().isEmpty())
        {
            ui->friendStackedWidget->setCurrentIndex(SearchFriendWidget);
        }
    }
    else
    {
        if (ui->lineEdit->text().isEmpty())
        {
            ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
        }
    }
}

void ChatWidget::onSignalChatBtn()
{
    ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
}

void ChatWidget::onSignalFriendListBtn()
{
    ui->friendStackedWidget->setCurrentIndex(FriendListWidget);
}

void ChatWidget::onSignalAddFriendBtn()
{
    ui->chatStackedWidget->SwitchToChatPage(AddFriendWid);
}

void ChatWidget::onSignalRecvFriendList(const QString& friendList)
{
    ChatWidgetManager::Instance()->onSignalRecvFriendList(friendList, m_mapUserInfo, m_vecFriendInfoWithC);
}

void ChatWidget::initUi()
{
    QSurfaceFormat format;
    if (optCoreProfile) {
        format.setVersion(4, 4);
        format.setProfile(QSurfaceFormat::CoreProfile);
    }
    if (optMultipleSample)
        format.setSamples(4);
    //m_ptrLastChatWidget->setFormat(format);

    //设置图标
    setWindowIcon(QIcon(":/LogInWidget/image/weixin.ico"));
    setWindowTitle(QString::fromLocal8Bit("秦"));

    //改变聊天输入框的颜色
    QPalette pal = ui->widget_3->palette();
    pal.setColor(QPalette::Window, QColor(238, 238, 238));
    ui->widget_3->setAutoFillBackground(true);
    ui->widget_3->setPalette(pal);
    ui->label->setVisible(false);
    ui->textEdit->setFontPointSize(16);

    //未添加和已添加的好友
    m_ptrNewFriendAndAreadyAddWidget->setSource(QUrl("qrc:/QML/QML/addFriend.qml"));
    m_ptrAddFriendQMLRoot = reinterpret_cast<QObject*>(m_ptrNewFriendAndAreadyAddWidget->rootObject());
    //qml的界面大小随quickwidget变化
    m_ptrNewFriendAndAreadyAddWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
    //把添加好友的界面加入到stackedwid中
    ui->chatStackedWidget->addWidget(m_ptrNewFriendAndAreadyAddWidget);
    ui->chatStackedWidget->insertToMap(AddFriendWid, m_ptrNewFriendAndAreadyAddWidget);


    //把空的界面加入到stackedwid,有时会用到空白界面
    m_ptrEmptyWid = new QQuickWidget();
    ui->chatStackedWidget->addWidget(m_ptrEmptyWid);
    ui->chatStackedWidget->insertToMap(EmptyWid, m_ptrEmptyWid);
    ui->chatStackedWidget->SwitchToChatPage(EmptyWid);


    //上次聊天列表设置qml文件
    m_ptrLastChatWidget->setSource(QUrl(QStringLiteral("qrc:/QML/QML/LastChatList.qml")));
    m_ptrLastChatWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
    //得到根对象
    m_ptrLastChatQMLRoot = reinterpret_cast<QObject*> (m_ptrLastChatWidget->rootObject());
    
    m_ptrFriendListWidget->setSource(QUrl("qrc:/QML/QML/friendList.qml"));
    m_ptrFriendListWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
    m_ptrFriendListQMLRoot = reinterpret_cast<QObject*>(m_ptrFriendListWidget->rootObject());
    
    //设置颜色
    ui->friendStackedWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->friendStackedWidget->addWidget(m_ptrLastChatWidget);
    ui->friendStackedWidget->addWidget(m_ptrFriendListWidget);
    ui->friendStackedWidget->addWidget(m_ptrSearchFriendList);
    ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
    ui->friendStackedWidget->setFocus();

    //初始化托盘图标对象
    m_ptrTrayIcon = new QSystemTrayIcon();
    m_ptrTrayIcon->setToolTip(QString::fromLocal8Bit("微信"));
    m_ptrTrayIcon->setIcon(QIcon(":/LogInWidget/image/icon.png"));
    m_ptrTrayIcon->show();
}

void ChatWidget::initConnect()
{
    //ui界面中的部件被点击
    connect(ui->textEdit, &MyTextEdit::signalTextEditIsFocus, this, &ChatWidget::onSignalTextEditIsFocus);
    connect(ui->pushButton, &QPushButton::clicked, this, &ChatWidget::onSignalSendMessage);

    //连接托盘图标的激活与对应动作的槽函数
    connect(m_ptrTrayIcon, &QSystemTrayIcon::activated, this, &ChatWidget::onSignalTrayTriggered);

    //连接添加好友界面同意信号
    connect(reinterpret_cast<QObject*>(m_ptrNewFriendAndAreadyAddWidget->rootObject()), SIGNAL(signalAgreeAdd(QString)), ChatWidgetManager::Instance().get(), SLOT(onSignalAgreeAddFriend(QString)));
    //主动添加好友信号处理
    connect(reinterpret_cast<QObject*>(m_ptrNewFriendAndAreadyAddWidget->rootObject()), SIGNAL(signalRequestAddFriend(QString,QString)), ChatWidgetManager::Instance().get(), SLOT(onSignalRequestAddFriend(QString,QString)));
    //上次聊天界面用户被点击
    connect(m_ptrLastChatQMLRoot, SIGNAL(signalFriendListClicked(QString)), this, SLOT(onSignalFriendListClicked(QString)));

    //收到好友消息列表后，由manager去处理数据
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvFriendListMessage, this, &ChatWidget::onSignalRecvFriendList);

    //收到服务端好友列表响应,既要初始化好友，也要初始化上次聊天列表 
    connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalGetFriendListFinished, this, &ChatWidget::initFriendList);
    connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalGetFriendListFinished, this, &ChatWidget::initLastChatList);
    connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalGetFriendListFinished, this, &ChatWidget::initAllChatWid);

    //侧边栏三个按钮的响应
    connect(ui->chatPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalChatBtn);
    connect(ui->friendListPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalFriendListBtn);
    connect(ui->addFriendPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalAddFriendBtn);
    connect(ui->lineEdit, &MyLineEdit::signalIsFocus, this, &ChatWidget::onSignalSearchTextLoseFocus);

    //收到好友同意请求后
    connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalAddFriendToLastChat, this, &ChatWidget::onSignalAdd2LastChat);
    //收到好友聊天消息
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvSingleChatMessage, this, &ChatWidget::onSignalSingleChatMessage);
    connect(TCPConnect::Instance().get(), &TCPConnect::signalNewFriendRequest, ChatWidgetManager::Instance().get(),&ChatWidgetManager::onSignalNewFriendRequest);
}

//************************************
// Method:    initData
// FullName:  ChatWidget::initData
// Access:    private 
// Returns:   void
// Qualifier: 进入聊天界面要进行初始化，获取一些数据
//************************************
void ChatWidget::initData()
{
    m_ptrFriendListWidget = new QQuickWidget();
    m_ptrNewFriendAndAreadyAddWidget = new QQuickWidget();
    m_ptrLastChatWidget = new QQuickWidget();
    m_ptrSearchFriendList = new QQuickWidget();
    ChatWidgetManager::Instance()->getLastChatListFromDB(m_vecLastChatFriend);
    ChatWidgetManager::Instance()->notifyServerOnline();
    ChatWidgetManager::Instance()->getFriendList();
}


void ChatWidget::initLastChatList()
{
    for (auto& item : m_vecLastChatFriend)
    {
        auto friendInfo=m_vecFriendInfoWithC[m_mapUserInfo[item]];
        //TODO获取上次聊天内容
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "addElementToModel", Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strName)), Q_ARG(QVariant, ""), Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strId)));
    }
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "initColor", Qt::DirectConnection);
}

void ChatWidget::initAllChatWid()
{
    for (auto& item : m_vecLastChatFriend)
    {
        initChatMessageWidAcordId(item);
    }
}

void ChatWidget::initFriendList()
{
    for (auto& item : m_vecFriendInfoWithC)
    {
        QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, ""), Q_ARG(QVariant, QString::fromStdString(item.m_strName)), Q_ARG(QVariant, QString::fromStdString(item.m_strId)), Q_ARG(QVariant, QString::fromStdString(item.m_strFirstChacter)));
    }
}

void ChatWidget::initChatMessageWidAcordId(QString strId)
{
    //创建和这个id的聊天界面，也就是加载qml，并保存在stackedwidget中
    MyChatMessageQuickWid* tmpWid = new MyChatMessageQuickWid();

    //获取聊天记录
    std::vector<MyChatMessageInfo> vecMyChatMessageInfo = ChatWidgetManager::Instance()->getChatMessageAcordIdAtInit(strId);

    tmpWid->setSource(QUrl(QStringLiteral("qrc:/QML/QML/chatMessage.qml")));
    tmpWid->setRootObj();
    tmpWid->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
    tmpWid->setClearColor(QColor(238, 238, 238));
    ui->chatStackedWidget->addWidget(tmpWid);
    //保存一下这个id对应的widget在stackedwidget中的位置，以便跳转使用
    ui->chatStackedWidget->insertToMap(strId.toInt(), tmpWid);

    //存储一下现在加载了的聊天记录数量，以便刷新时知道从哪个位置再加载
    tmpWid->addRecordCount(vecMyChatMessageInfo.size());
    //保存一下这个页面所对应的好友的id
    tmpWid->SetUserId(strId);

    //把聊天记录加载进去
    for (auto& item : vecMyChatMessageInfo)
    {
        QMetaObject::invokeMethod(tmpWid->getRootObj(), "insertMessageModel", Q_ARG(QVariant, (item.m_strName)), Q_ARG(QVariant, (item.m_strMessage)), Q_ARG(QVariant, item.m_bIsSelf), Q_ARG(QVariant, (item.m_strName.mid(0, 1))), Q_ARG(QVariant, strId));
        tmpWid->setInitial(item.m_strName.mid(0, 1));
        tmpWid->setUserName(item.m_strName);
    }

    QMetaObject::invokeMethod(tmpWid->getRootObj(), "scrollToEnd", Qt::DirectConnection);
}

//向服务器发送初始化消息，告知此id在线
/*void ChatWidget::notifyServerOnline()
{
    InitialRequestJsonData initialJosnData;
    initialJosnData.m_strId = m_strUserId.toStdString();
    std::string sendMessage = initialJosnData.generateJson();
    TCPConnect::Instance()->sendMessage(sendMessage);
}*/

ChatWidget::~ChatWidget()
{
    delete ui;
    delete m_ptrFriendListQMLRoot;
    m_ptrFriendListQMLRoot = nullptr;
    delete m_ptrLastChatQMLRoot;
    m_ptrLastChatQMLRoot = nullptr;
    delete m_ptrNullMessageTimer;
    m_ptrNullMessageTimer = nullptr;
    delete m_ptrLastChatWidget;
    m_ptrLastChatWidget = nullptr;
    delete m_ptrFriendListWidget;
    m_ptrFriendListWidget = nullptr;
    delete m_ptrSearchFriendList;
    m_ptrSearchFriendList = nullptr;
    delete m_ptrTrayIcon;
    m_ptrTrayIcon = nullptr;
    delete m_ptrEmptyWid;
    m_ptrEmptyWid = nullptr;
    if (m_ptrNullMessageTimer != nullptr && m_ptrNullMessageTimer->isActive())
    {
        m_ptrNullMessageTimer->stop();
    }
    delete m_ptrNullMessageTimer;
    m_ptrNullMessageTimer = nullptr;
}

