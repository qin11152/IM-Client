﻿#include "../../ui/ChatWidget/ChatWidget.h"
#include "MytextEdit/MyTextEdit.h"
#include "TCPConnect/TCPConnect.h"
#include "MyLineEdit/MyLineEdit.h"
#include "DataBaseDelegate/DataBaseDelegate.h"
#include "protocol/InitialRequestJsonData/InitialRequestJsonData.h"
#include "../../protocol/ChatMessageJsonData/SingleChatMessageJsonData.h"
#include "ui_ChatWidget.h"
#include <QSurfaceFormat>
#include <QDebug>

const QString ChatRecordTable = "chatrecord";
static bool optMultipleSample = false;
static bool optCoreProfile = false;

ChatWidget::ChatWidget(int id, QWidget* parent)
    : QWidget(parent),
    m_iId(id)
{
    ui = new Ui::ChatWidget();
    ui->setupUi(this);
    DataBaseDelegate::Instance()->SetUserId(m_iId);
    initUi();
    initData();
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
        int id = tmpWid->GetUserId();

        //通过网络将信息发送出去
        SingleChatMessageJsonData singleChatData;
        singleChatData.m_strSendUserId = std::to_string(id);
        singleChatData.m_strRecvUserId = std::to_string(id);
        singleChatData.m_strMessage = lineEditMessage.toStdString();
        std::string sendMessage = singleChatData.generateJson();
        //printf("%s\n",sendMessage.c_str());
        TCPConnect::Instance()->sendMessage(sendMessage);

        //更新到界面中
        QMetaObject::invokeMethod(tmpWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, tmpWid->getUserName()), Q_ARG(QVariant, lineEditMessage), Q_ARG(QVariant, true), Q_ARG(QVariant, tmpWid->GetInitial()), Q_ARG(QVariant, id));
        //把消息更新到界面中
        ui->textEdit->clear();
    }
}

void ChatWidget::onSignalSwitchFriendListWidget(int type)
{
    ui->friendStackedWidget->setCurrentIndex(type);
}

void ChatWidget::onSignalFriendListClicked(int iId)
{
    //TODO打开对应的聊天记录页面
    ui->chatStackedWidget->SwitchToChatPage(iId);
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

void ChatWidget::onSignalSingleChatMessage(QString& chatMessage)
{
    SingleChatMessageJsonData singleChatData(chatMessage.toStdString());
    auto tmpWid = static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->getWidAcord2Id(atoi(singleChatData.m_strRecvUserId.c_str())));
    QMetaObject::invokeMethod(tmpWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, tmpWid->getUserName()), Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strMessage)), Q_ARG(QVariant, false), Q_ARG(QVariant, tmpWid->GetInitial()), Q_ARG(QVariant, atoi(singleChatData.m_strRecvUserId.c_str())));
}

void ChatWidget::onSignalAgreeAddFriend(QString friendName)
{
    qDebug() << friendName;
    //TODO从数据库查找用户名对应id并转移到已经添加的好友表中
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
    //m_ptrFriendListWidget->setFormat(format);

    //设置图标
    setWindowIcon(QIcon(":/LogInWidget/image/weixin.ico"));
    setWindowTitle(QString::fromLocal8Bit("秦"));

    QPalette pal = ui->widget_3->palette();
    pal.setColor(QPalette::Window, QColor(238, 238, 238));
    ui->widget_3->setAutoFillBackground(true);
    ui->widget_3->setPalette(pal);
    ui->label->setVisible(false);
    ui->textEdit->setFontPointSize(16);

    m_ptrSearchWidget = new QQuickWidget();
    m_ptrNewFriendAndAreadyAddWidget = new QQuickWidget();
    m_ptrFriendListWidget = new QQuickWidget();

    m_ptrNewFriendAndAreadyAddWidget->setSource(QUrl("qrc:/QML/QML/addFriend.qml"));
    //qml的界面大小随quickwidget变化
    m_ptrNewFriendAndAreadyAddWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
    //把添加好友的界面加入到stackedwid中
    ui->chatStackedWidget->addWidget(m_ptrNewFriendAndAreadyAddWidget);
    ui->chatStackedWidget->insertToMap(AddFriendWid, m_ptrNewFriendAndAreadyAddWidget);


    //好友列表设置qml文件
    m_ptrFriendListWidget->setSource(QUrl(QStringLiteral("qrc:/QML/QML/LastChatList.qml")));
    m_ptrFriendListWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);

    //得到根对象
    m_ptrFriendListQMLRoot = reinterpret_cast<QObject*> (m_ptrFriendListWidget->rootObject());
    //设置颜色
    ui->friendStackedWidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->friendStackedWidget->addWidget(m_ptrFriendListWidget);
    ui->friendStackedWidget->addWidget(m_ptrSearchWidget);
    ui->friendStackedWidget->setCurrentIndex(FriendListWidget);
    ui->friendStackedWidget->setFocus();

    QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, "qin"), Q_ARG(QVariant, ""), Q_ARG(QVariant, 1));
    QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, "apple"), Q_ARG(QVariant, ""), Q_ARG(QVariant, 2));
    QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, "sum"), Q_ARG(QVariant, ""), Q_ARG(QVariant, 3));

    //初始化托盘图标对象
    m_ptrTrayIcon = new QSystemTrayIcon();
    m_ptrTrayIcon->setToolTip(QString::fromLocal8Bit("微信"));
    m_ptrTrayIcon->setIcon(QIcon(":/LogInWidget/image/icon.png"));
    m_ptrTrayIcon->show();
}

void ChatWidget::initConnect()
{
    connect(ui->textEdit, &MyTextEdit::signalTextEditIsFocus, this, &ChatWidget::onSignalTextEditIsFocus);
    connect(ui->pushButton, &QPushButton::clicked, this, &ChatWidget::onSignalSendMessage);
    connect(ui->lineEdit, &MyLineEdit::signalSwitchStackedWidget, this, &ChatWidget::onSignalSwitchFriendListWidget);
    connect(m_ptrFriendListQMLRoot, SIGNAL(signalFriendListClicked(int)), this, SLOT(onSignalFriendListClicked(int)));
    //连接托盘图标的激活与对应动作的槽函数
    connect(m_ptrTrayIcon, &QSystemTrayIcon::activated, this, &ChatWidget::onSignalTrayTriggered);
    //连接添加好友界面同意信号
    connect(reinterpret_cast<QObject*>(m_ptrNewFriendAndAreadyAddWidget->rootObject()), SIGNAL(signalAgreeAdd(QString)), this, SLOT(onSignalAgreeAddFriend(QString)));
    connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        ui->chatStackedWidget->SwitchToChatPage(AddFriendWid);
        });
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
    getLastChatListFromDB();
    notifyServerOnline();
}

//************************************
// Method:    getLastChatListFromDB
// FullName:  ChatWidget::getLastChatListFromDB
// Access:    private 
// Returns:   void
// Qualifier: 从数据库获取上次聊天信息保存在map中，key是位置，value是id，按上次位置从上到下排列
//************************************
void ChatWidget::getLastChatListFromDB()
{
    //map结构体，存储上次关闭时聊天列表中的顺序
    std::map<int, int> LastChatInfo;
    DataBaseDelegate::Instance()->queryLastChatListFromDB(LastChatInfo);

    //遍历map
    for (auto& item : LastChatInfo)
    {
        int id = item.second;
        //初始化聊天列表,从数据库中得到聊天列表的人
        
        //先看这个id的聊天记录有多少
        int iMessageCount = DataBaseDelegate::Instance()->GetChatRecordCountFromDB(id);
        //如果大于10条就加载十条，小于十条就有多少加载多少
        int needLoadCount = std::min(10, iMessageCount);
        //获取聊天记录
        std::vector<MyChatMessageInfo> vecMyChatMessageInfo;
        DataBaseDelegate::Instance()->queryChatRecordAcodIdFromDB(id, vecMyChatMessageInfo, needLoadCount);

        //创建和这个id的聊天界面，也就是加载qml，并保存在stackedwidget中
        MyChatMessageQuickWid* tmpWid = new MyChatMessageQuickWid();
        tmpWid->setSource(QUrl(QStringLiteral("qrc:/QML/QML/chatMessage.qml")));
        tmpWid->setRootObj();
        tmpWid->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
        tmpWid->setClearColor(QColor(238, 238, 238));
        ui->chatStackedWidget->addWidget(tmpWid);
        //保存一下这个id对应的widget在stackedwidget中的位置，以便跳转使用
        ui->chatStackedWidget->insertToMap(id, tmpWid);

        //存储一下现在加载了的聊天记录数量，以便刷新时知道从哪个位置再加载
        tmpWid->setRecordCount(needLoadCount);
        tmpWid->SetUserId(id);

        //把聊天记录加载进去
        for (auto& item : vecMyChatMessageInfo)
        {
            QMetaObject::invokeMethod(tmpWid->getRootObj(), "insertMessageModel", Q_ARG(QVariant, (item.m_strName)), Q_ARG(QVariant, (item.m_strMessage)), Q_ARG(QVariant, item.m_bIsSelf),Q_ARG(QVariant, (item.m_strName.mid(0,1))),Q_ARG(QVariant,id));
            tmpWid->setInitial(item.m_strName.mid(0, 1));
            tmpWid->setUserName(item.m_strName);
        }

        QMetaObject::invokeMethod(tmpWid->getRootObj(), "scrollToEnd", Qt::DirectConnection);
    }
    ui->chatStackedWidget->SwitchToChatPageAcordIndex(1);
}

//向服务器发送初始化消息，告知此id在线
void ChatWidget::notifyServerOnline()
{
    InitialRequestJsonData initialJosnData;
    initialJosnData.m_strId = std::to_string(m_iId);
    std::string sendMessage = initialJosnData.generateJson();
    TCPConnect::Instance()->sendMessage(sendMessage);
}

ChatWidget::~ChatWidget()
{
    delete ui;
    delete m_ptrFriendListQMLRoot;
    m_ptrFriendListQMLRoot = nullptr;
    delete m_ptrNullMessageTimer;
    m_ptrNullMessageTimer = nullptr;
    delete m_ptrFriendListWidget;
    m_ptrFriendListWidget = nullptr;
    delete m_ptrSearchWidget;
    m_ptrSearchWidget = nullptr;
    delete m_ptrTrayIcon;
    m_ptrTrayIcon = nullptr;
}

