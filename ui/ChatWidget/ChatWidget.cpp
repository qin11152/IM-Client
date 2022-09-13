#include "../../ui/ChatWidget/ChatWidget.h"
#include "MytextEdit/MyTextEdit.h"
//#include "TCPConnect/TCPConnect.h"
#include "MyLineEdit/MyLineEdit.h"
#include "DataBaseDelegate/DataBaseDelegate.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"
#include "module/Log/Log.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "protocol/ChatMessageJsonData/SingleChatMessageJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "ui_ChatWidget.h"
#include <QPushButton>
#include <QDir>
#include <ctime>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSound>

const QString ChatRecordTable = "chatrecord";

ChatWidget::ChatWidget(QString id, QString name, QWidget* parent)
    : QWidget(parent),
      m_strUserId(id),
      m_strUserName(name)
{
    ui = new Ui::ChatWidget();
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
    PublicDataManager::get_mutable_instance().setMyId(id);
    PublicDataManager::get_mutable_instance().setMyName(name);
    ChatWidgetManager::Instance()->setUserId(m_strUserId);
    ChatWidgetManager::Instance()->setUserName(m_strUserName);
    ChatWidgetManager::Instance()->initDBOperateThread();
    DataBaseDelegate::Instance()->setUserId(m_strUserId);
    DataBaseDelegate::Instance()->init();
    initData();
    initUi();
    ChatWidgetManager::Instance()->setQMLRootPtr(m_ptrAddFriendQMLRoot, m_ptrFriendListQMLRoot, m_ptrLastChatQMLRoot);
    initConnect();
}

MyChatMessageQuickWid* ChatWidget::getChatMsgWidAcordId(QString id)
{
    return static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->getWidAcord2Id(id.toInt()));
}

bool ChatWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_ptrTrayIcon)
    {
        if (event->type() == QMouseEvent::HoverEnter)
        {
            printf("enter\n");
        }
        if (event->type() == QMouseEvent::HoverLeave)
        {
            printf("leave\n");
        }
    }
    return QWidget::eventFilter(watched, event);
}

//添加了新好友后，增加好友信息到lastchat中,调用的是插入方法
void ChatWidget::onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo)
{
    QString imagePath = "";
    if ("" == friendInfo.m_strImagePath)
    {
        imagePath = kDefaultProfileImage;
    }
    else 
    {
        imagePath = QString::fromStdString(friendInfo.m_strImagePath);
    }
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "insertElementToModel",
                              Q_ARG(QVariant, imagePath),
                              Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strName)),
                              Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strId)));
    auto tmp = MyLastChatFriendInfo();
    tmp.m_strId = QString::fromStdString(friendInfo.m_strId);
    tmp.m_strName = QString::fromStdString(friendInfo.m_strName);
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "switchToItemAndChangeColor", Q_ARG(QVariant, tmp.m_strId));
    initChatMessageWidAcordId(tmp);
    //chatstackwidget也要跳转到新来的这个界面
    ui->chatStackedWidget->SwitchToChatPage(tmp.m_strId.toInt());
    //新添加的好友，也创建一条聊天记录表
    DataBaseDelegate::Instance()->createUserChatTable(QString::fromStdString(friendInfo.m_strId));
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
                this->m_ptrNullMessageTimer->stop();
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
        time_t now = time(nullptr);
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
        singleChatData.m_strSendName = m_strUserName.toStdString();
        std::string sendMessage = singleChatData.generateJson();
        //printf("%s\n",sendMessage.c_str());
        //TCPConnect::Instance()->sendMessage(sendMessage);
        emit signalSendMsg(sendMessage);

        auto tablename = "chatrecord" + singleChatData.m_strRecvUserId;
        //查看数据库中这个表是否存在
        int totalCnt = tmpWid->getTotalRecordCount();
        if (DataBaseDelegate::Instance()->isTableExist(QString::fromStdString(tablename)))
        {
            //添加到数据库
            DataBaseDelegate::Instance()->insertChatRecoed(totalCnt, singleChatData.m_strRecvUserId.c_str(),
                                                           QString::fromStdString(singleChatData.m_strMessage),
                                                           QString::fromStdString(singleChatData.m_strTime), true,
                                                           m_strUserName);
        }
        else
        {
            //没有就创建这个表
            DataBaseDelegate::Instance()->createUserChatTable(QString::fromStdString(singleChatData.m_strRecvUserId));
            //添加到数据库
            DataBaseDelegate::Instance()->insertChatRecoed(totalCnt, singleChatData.m_strRecvUserId.c_str(),
                                                           QString::fromStdString(singleChatData.m_strMessage),
                                                           QString::fromStdString(singleChatData.m_strTime), true,
                                                           m_strUserName);
        }

        //更新到界面中
        QMetaObject::invokeMethod(tmpWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, m_strUserName),
                                  Q_ARG(QVariant, lineEditMessage), Q_ARG(QVariant, true),
                                  Q_ARG(QVariant, m_strUserName.mid(0, 1)), Q_ARG(QVariant, m_strUserId),Q_ARG(QVariant,""));
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "updateLastChatMsg", Q_ARG(QVariant, lineEditMessage),Q_ARG(QVariant, id));
        //把消息更新到界面中
        ui->textEdit->clear();
        tmpWid->addTotalAndCurrentRecordCount(1);
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "findPosInModelAndMove2Top", Q_ARG(QVariant, id));
        ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
        //因为位置改变了，要重新排列顺序
        //看位置是否发生了变化再决定是否更新数据库
    }
}

void ChatWidget::onSignalLastChatItemClicked(const QString strId)
{
    if (ui->chatStackedWidget->isWidCreate(strId.toInt()))
    {
        //打开对应的聊天记录页面
        ui->chatStackedWidget->SwitchToChatPage(strId.toInt());
        auto tmp = static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->currentWidget());
        ui->nickNameLabel->setText(tmp->getUserName());
        onSignalHideRedRectangleInLastChat(strId);
    }
}

//点击好友列表之后，要把添加到lastchat中，并且改变lastchat中的颜色
void ChatWidget::onSignalFriendListItemClicked(QString strId, QString name)
{
    //左侧列表界面设为lastchat
    ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
    //如果上次聊天列表中没有这个人，还要添加到上次聊天列表中
    //auto friendInfo = m_vecFriendInfoWithC[m_mapUserInfo[strId]];
    auto friendVec = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec();
    auto friendInfo = friendVec[PublicDataManager::get_mutable_instance().getMyUsetInfoMap()[strId]];
    //获取上次聊天内容
    QString imagePath = "";
    if ("" == friendInfo.m_strImagePath)
    {
        imagePath = kDefaultProfileImage;
    }
    else
    {
        imagePath = QString::fromStdString(friendInfo.m_strImagePath);
    }
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "judgeAndInsertToModel", Q_ARG(QVariant, name.mid(0, 1)),
                              Q_ARG(QVariant, name), Q_ARG(QVariant, strId));

    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "switchToItemAndChangeColor", Q_ARG(QVariant, strId));

    //右侧聊天界面stackwidget没有的话，也要创建
    if (!ui->chatStackedWidget->isWidCreate(atoi(strId.toStdString().c_str())))
    {
        auto tmp = MyLastChatFriendInfo();
        tmp.m_strId = strId;
        tmp.m_strName = name;
        initChatMessageWidAcordId(tmp);
        //插入到上次聊天的数据库中
        DataBaseDelegate::Instance()->insertLastChat(tmp.m_strId);
    }
    //去除掉红点
    onSignalHideRedRectangleInLastChat(strId);
    ui->chatStackedWidget->SwitchToChatPage(strId.toInt());
    const auto tmp = static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->currentWidget());
    ui->nickNameLabel->setText(tmp->getUserName());
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
    QApplication::alert(this);
    //QSound::play(":/res/sound/message.wav");
    const SingleChatMessageJsonData singleChatData(chatMessage.toStdString());
    const QString sendId =QString::fromStdString(singleChatData.m_strSendUserId);
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setLastChatRecord", Q_ARG(QVariant, chatMessage),Q_ARG(QVariant, sendId));
    //存储在数据库中
    auto tablename = "chatrecord" + singleChatData.m_strSendUserId;
    if (!ui->chatStackedWidget->isWidCreate(std::atoi(singleChatData.m_strSendUserId.c_str())))
    {
        auto tmp = MyLastChatFriendInfo();
        tmp.m_strId = QString::fromStdString(singleChatData.m_strSendUserId);
        tmp.m_strName = QString::fromStdString(singleChatData.m_strSendName);
        initChatMessageWidAcordId(tmp);
    }
    auto tmpWid = static_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->getWidAcord2Id(
        atoi(singleChatData.m_strSendUserId.c_str())));
    const int totalCnt = tmpWid->getTotalRecordCount();
    if (DataBaseDelegate::Instance()->isTableExist(QString::fromStdString(tablename)))
    {
        //添加到数据库
        DataBaseDelegate::Instance()->insertChatRecoed(totalCnt, singleChatData.m_strSendUserId.c_str(),
                                                       QString::fromStdString(singleChatData.m_strMessage),
                                                       QString::fromStdString(singleChatData.m_strTime), false,
                                                       QString::fromStdString(singleChatData.m_strSendName));
    }
    else
    {
        //没有就创建这个表
        DataBaseDelegate::Instance()->createUserChatTable(QString::fromStdString(singleChatData.m_strSendUserId));
        //添加到数据库
        DataBaseDelegate::Instance()->insertChatRecoed(totalCnt, singleChatData.m_strSendUserId.c_str(),
                                                       QString::fromStdString(singleChatData.m_strMessage),
                                                       QString::fromStdString(singleChatData.m_strTime), false,
                                                       QString::fromStdString(singleChatData.m_strSendName));
    }
    //如果当前的chat页面不是来消息的这个人，就开始闪烁，并显示红点，是的话就直接更新到界面
    if (ui->chatStackedWidget->currentWidget() != tmpWid)
    {
        onSignalSetRedRectangleShow(tmpWid);
        onSignalTrayIconTwinkle(true);
    }
    QMetaObject::invokeMethod(tmpWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strSendName)),
        Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strMessage)),
        Q_ARG(QVariant, false), Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strSendName).mid(0, 1)),
        Q_ARG(QVariant, atoi(singleChatData.m_strRecvUserId.c_str())),Q_ARG(QVariant,""));
    tmpWid->addTotalAndCurrentRecordCount(1);
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
    ui->chatStackedWidget->SwitchToChatPage(EmptyWid);
}

void ChatWidget::onSignalAddFriendBtn()
{
    ui->chatStackedWidget->SwitchToChatPage(AddFriendWid);
}

void ChatWidget::onSignalSideBarProfileImageBtn()
{
    onSignalChatWidOpenProfileImagePreview(m_strUserId.toInt());
}

void ChatWidget::onSignalRecvFriendList(const QString& friendList)
{
    ChatWidgetManager::Instance()->onSignalRecvFriendList(friendList, PublicDataManager::get_mutable_instance().getMyUsetInfoMap(), PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec());
}

void ChatWidget::onSignalUpdateChatMessage(const QString id)
{
    //先根据id找到对应的chatmessagequickwid
    const auto tmpWid = dynamic_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->getWidAcord2Id(id.toInt()));
    //然后根据wid得到现在还有多少记录可以加载
    //用总的记录减去已加载的数量
    int curCount = tmpWid->getRecordCount();
    int canLoadCount = tmpWid->getTotalRecordCount() - tmpWid->getRecordCount();
    //聊天记录如果没有就不更新，有且大于10条就更新10 条
    canLoadCount = canLoadCount > 10 ? 10 : canLoadCount;
    std::vector<MyChatMessageInfo> vecMyChatMessageInfo;

    if (0 == canLoadCount)
    {
        return;
    }
    //查询聊天记录的起始位置是聊天页面当前的数量,加载的总数量是canLoadCount
    DataBaseDelegate::Instance()->queryChatRecordAcodIdFromDB(id, vecMyChatMessageInfo, canLoadCount,
                                                              tmpWid->getRecordCount());
    //存储一下现在页面中加载了的聊天记录数量，以便刷新时知道从哪个位置再加载
    tmpWid->addCurrentRecordCount(vecMyChatMessageInfo.size());

    //把聊天记录加载进去
    for (const auto& item : vecMyChatMessageInfo)
    {
        QString strId = "";
        if (item.m_bIsSelf)
        {
            strId = m_strUserId;
        }
        else
        {
            strId = id;
        }
        QMetaObject::invokeMethod(tmpWid->getRootObj(), "insertMessageModel", Q_ARG(QVariant, (item.m_strName)),
                                  Q_ARG(QVariant, (item.m_strMessage)), Q_ARG(QVariant, item.m_bIsSelf),
                                  Q_ARG(QVariant, (item.m_strName.mid(0, 1))), Q_ARG(QVariant, strId),Q_ARG(QVariant,/*QString::fromStdString(PublicDataManager::get_mutable_instance().getFriendInfoAcordId(id).m_strImagePath)*/"qrc:///LogInWidget/image/lv.jpg"));
    }
    QMetaObject::invokeMethod(tmpWid->getRootObj(), "scrollToPosition", Q_ARG(QVariant, curCount));
}

void ChatWidget::initUi()
{
    //设置图标
    //setWindowIcon(QIcon(":/LogInWidget/image/weixin.ico"));
    setWindowTitle(QString::fromLocal8Bit("q微信"));

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
    m_ptrLastChatQMLRoot = reinterpret_cast<QObject*>(m_ptrLastChatWidget->rootObject());

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
    m_ptrTrayIcon->installEventFilter(this);

    setProfileImage(kDefaultProfileImageWidget);
}

void ChatWidget::initConnect()
{
    //ui界面中的部件被点击
    connect(ui->textEdit, &MyTextEdit::signalTextEditIsFocus, this, &ChatWidget::onSignalTextEditIsFocus);
    connect(ui->pushButton, &QPushButton::clicked, this, &ChatWidget::onSignalSendMessage);
    //QML页面通知要更新lastchat了
    connect(m_ptrLastChatQMLRoot, SIGNAL(signalNeedUpdateLastChat()), this, SLOT(onSignalNeedUpdateLastChat()));
    //连接QML页面添加好友界面同意信号
    connect(reinterpret_cast<QObject*>(m_ptrNewFriendAndAreadyAddWidget->rootObject()), SIGNAL(signalAgreeAdd(QString)),
        ChatWidgetManager::Instance().get(), SLOT(onSignalAgreeAddFriend(QString)));
    //QML页面主动添加好友信号处理
    connect(reinterpret_cast<QObject*>(m_ptrNewFriendAndAreadyAddWidget->rootObject()),
        SIGNAL(signalRequestAddFriend(QString, QString)), ChatWidgetManager::Instance().get(),
        SLOT(onSignalRequestAddFriend(QString, QString)));
    //QML页面上次聊天界面用户被点击
    connect(m_ptrLastChatQMLRoot, SIGNAL(signalFriendListClicked(QString)), this,
        SLOT(onSignalLastChatItemClicked(QString)));
    //QML页面好友列表被点击
    connect(m_ptrFriendListQMLRoot, SIGNAL(signalFriendListClicked(QString, QString)), this,
        SLOT(onSignalFriendListItemClicked(QString, QString)));

    //连接托盘图标的激活与对应动作的槽函数
    connect(m_ptrTrayIcon, &QSystemTrayIcon::activated, this, &ChatWidget::onSignalTrayTriggered);
    //托盘闪烁定时器到时槽函数
    connect(m_ptrIconTwinkleTimer, &QTimer::timeout, this, &ChatWidget::onSignalIconTwinkleTimerout);

    //收到服务端好友列表响应,既要初始化好友，也要初始化上次聊天列表 
    connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalGetFriendListFinished, this,
            &ChatWidget::initFriendList);
    //收到服务端好友同意请求后
    connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalBecomeFriend, this,
        &ChatWidget::onSignalBecomeFriend);
#if 0
    //收到好友消息列表后，由manager去处理数据
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvFriendListMessage, this,
        &ChatWidget::onSignalRecvFriendList);
    //收到服务端好友聊天消息
    connect(TCPConnect::Instance().get(), &TCPConnect::signalRecvSingleChatMessage, this,
        &ChatWidget::onSignalSingleChatMessage);
    //收到服务端好友添加请求
    connect(TCPConnect::Instance().get(), &TCPConnect::signalNewFriendRequest, ChatWidgetManager::Instance().get(),
        &ChatWidgetManager::onSignalNewFriendRequest);
    //收到服务端同意好友添加
    connect(TCPConnect::Instance().get(), &TCPConnect::signalBecomeFriendNotify, ChatWidgetManager::Instance().get(),
        &ChatWidgetManager::onSignalBecomeFriend);
#endif
    //收到好友消息列表后，由manager去处理数据
    connect(m_ptrTCPThread, &TCPThread::signalRecvFriendListMessage, this,
        &ChatWidget::onSignalRecvFriendList,Qt::QueuedConnection);
    //收到服务端好友聊天消息
    connect(m_ptrTCPThread, &TCPThread::signalRecvSingleChatMessage, this,
        &ChatWidget::onSignalSingleChatMessage, Qt::QueuedConnection);
    //收到服务端好友添加请求
    connect(m_ptrTCPThread, &TCPThread::signalNewFriendRequest, ChatWidgetManager::Instance().get(),
        &ChatWidgetManager::onSignalNewFriendRequest, Qt::QueuedConnection);
    //收到服务端同意好友添加
    connect(m_ptrTCPThread, &TCPThread::signalBecomeFriendNotify, ChatWidgetManager::Instance().get(),
        &ChatWidgetManager::onSignalBecomeFriend, Qt::QueuedConnection);
    //子线程发送消息
    connect(this, &ChatWidget::signalSendMsg, m_ptrTCPThread, &TCPThread::sendMessage, Qt::QueuedConnection);
    //子线程发送图片消息
    connect(this, &ChatWidget::signalSendImageMsg, m_ptrTCPThread, &TCPThread::sendImageMsg, Qt::QueuedConnection);

    //侧边栏三个按钮的响应
    connect(ui->chatPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalChatBtn);
    connect(ui->friendListPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalFriendListBtn);
    connect(ui->addFriendPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalAddFriendBtn);
    connect(ui->profileImageButton, &QPushButton::clicked, this, &ChatWidget::onSignalSideBarProfileImageBtn);

    connect(ui->lineEdit, &MyLineEdit::signalIsFocus, this, &ChatWidget::onSignalSearchTextLoseFocus);
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
    m_ptrTCPThread = new TCPThread();

    m_ptrFriendListWidget = new QQuickWidget();
    m_ptrNewFriendAndAreadyAddWidget = new QQuickWidget();
    m_ptrLastChatWidget = new QQuickWidget();
    m_ptrSearchFriendList = new QQuickWidget();

    m_ptrIconTwinkleTimer=new QTimer();

    //先去从缓存的数据库中获取到相关的数据并更新到实际的数据库，然后在执行相关操作
    auto dataBase = QSqlDatabase::addDatabase("QSQLITE", "sqlite2");
    connectToBackupDB(dataBase);
    std::vector<QString> tmpOrder;
    getLastChatFromBackup(tmpOrder, dataBase);
    disConnectBackupDB(dataBase);

    //清空上次聊天数据库中
    DataBaseDelegate::Instance()->clearLastChat();
    //写入到自己的数据库中
    DataBaseDelegate::Instance()->insertLastChat(tmpOrder);

    //这时候主线程对备份数据库操作完成了，子线程可以连接了
    ChatWidgetManager::Instance()->initDBThreadConnect();
    ChatWidgetManager::Instance()->getLastChatListFromDB(PublicDataManager::get_mutable_instance().getMyLastChatFriendInfoVec());
    ChatWidgetManager::Instance()->notifyServerOnline();
    //ChatWidgetManager::Instance()->getFriendList();
}

void ChatWidget::getLastChatFromBackup(std::vector<QString>& tmpOrder, QSqlDatabase& db)
{
    const QString str="select * from lastchatlist"+m_strUserId;
    QSqlQuery query(db);
    if(!query.exec(str))
    {
        _LOG(Logcxx::Level::ERRORS, "getLastChatFromBackup error");
    }
    QSqlRecord record = query.record();
    while (query.next())
    {
        auto tmp = MyLastChatFriendInfo();
        record = query.record();
        tmpOrder.push_back(record.value("id").toString());
    }
}

void ChatWidget::connectToBackupDB(QSqlDatabase& db)
{
    //文件夹路径
    const QString fileName = QApplication::applicationDirPath() + "/data";
    //库名称
    const QString dataName = QApplication::applicationDirPath() + "/data/thread" + ".db";
    db.setDatabaseName(dataName);
    db.open();
}

void ChatWidget::disConnectBackupDB(QSqlDatabase& db)const
{
    db.close();
    const QString fileName = QApplication::applicationDirPath() + "/data";
    //库名称
    const QString dataName = QApplication::applicationDirPath() + "/data/thread" + ".db";
    db.removeDatabase(dataName);
}

//收到服务器返回的好友列表后初始化lastchat
void ChatWidget::initLastChatList()
{
    for (auto& item : PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec())
    {
        auto friendInfo = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec()[PublicDataManager::get_mutable_instance().getMyUsetInfoMap()[QString::fromStdString(item.m_strId)]];
        //获取上次聊天内容
        QString imagePath = "";
        if ("" == friendInfo.m_strImagePath)
        {
            imagePath = kDefaultProfileImage;
        }
        else
        {
            imagePath = QString::fromStdString(friendInfo.m_strImagePath);
        }
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "addElementToModel",
                                  Q_ARG(QVariant, imagePath),
                                  Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strName)),
                                  Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strId)),
            Q_ARG(QVariant, DataBaseDelegate::Instance()->queryLastChatRecord(QString::fromStdString(item.m_strId))));
        //QMetaObject::invokeMethod(m_ptrLastChatQMLRoot,"setLastChatRecord",Q_ARG(QVariant,DataBaseDelegate::Instance()->queryLastChatRecord(item.m_strId)));
    }
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "initColor", Qt::DirectConnection);
}

void ChatWidget::initAllChatWid()
{
    for (auto& item : PublicDataManager::get_mutable_instance().getMyLastChatFriendInfoVec())
    {
        initChatMessageWidAcordId(item);
    }
}

void ChatWidget::onSignalHideRedRectangleInLastChat(const QString id)
{
    const auto tmpWidget = reinterpret_cast<MyChatMessageQuickWid*>(ui->chatStackedWidget->getWidAcord2Id(id.toInt()));
    tmpWidget->clearUnreadMsgCnt();
    QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setRedMsgAndHide",Q_ARG(QVariant, id));
    onSignalTrayIconTwinkle(false);
    m_iTrayState = TrayIconState::Normal;
}

void ChatWidget::onSignalSetRedRectangleShow(MyChatMessageQuickWid* ptr)const
{
    if (ptr != ui->chatStackedWidget->currentWidget())
    {
        ptr->setUnreadMsgCnt();
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setRedMsgAndShow", Q_ARG(QVariant, ptr->GetUserId()),
            Q_ARG(QVariant, QString::number(ptr->getUnreadMsgCnt())));
    }
}

//当有好友同意了添加请求，对应的处理
void ChatWidget::onSignalBecomeFriend(const MyFriendInfoWithFirstC& friendInfo)
{
    //要添加到界面的上次聊天列表中
    onSignalAdd2LastChat(friendInfo);
    //要添加到内存的好友列表中
    onAddFriendIntoList(friendInfo);
    //要把好友列表的界面也更新一下
    onUpdateFriendListUI();
}

void ChatWidget::onAddFriendIntoList(const MyFriendInfoWithFirstC& friendInfo)
{
    PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().push_back(friendInfo);
}

void ChatWidget::onSortFriendList()
{
    std::ranges::sort(PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().begin(), PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().end(), [&](const MyFriendInfoWithFirstC& l, const MyFriendInfoWithFirstC& r)
    {
            return l.m_strFirstChacter < r.m_strFirstChacter;
    });
}

void ChatWidget::onUpdateFriendListUI() const
{
    //先清空qml中的列表
    QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "clearModel");

    //然后再按照顺序添加进去
    for(const auto& item: PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec())
    {
        QString imagePath = "";
        if ("" == item.m_strImagePath)
        {
            imagePath = kDefaultProfileImage;
        }
        else
        {
            imagePath = QString::fromStdString(item.m_strImagePath);
        }
        QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, imagePath),
            Q_ARG(QVariant, QString::fromStdString(item.m_strName)),
            Q_ARG(QVariant, QString::fromStdString(item.m_strId)),
            Q_ARG(QVariant, QString::fromStdString(item.m_strFirstChacter)));
    }
}

void ChatWidget::onSignalNeedUpdateLastChat()const
{
    ChatWidgetManager::Instance()->onSignalUpdateLastChat();
}

void ChatWidget::onSignalChatWidProfileImageClicked(const QString id)
{
    onSignalChatWidOpenProfileImagePreview(id.toInt());
}

//底部栏闪烁槽函数
//parm[IN] bNeed true:闪烁，false:不闪烁
void ChatWidget::onSignalTrayIconTwinkle(const bool bNeed)
{
    if(bNeed)
    {
        if(!m_ptrIconTwinkleTimer)
        {
            m_ptrIconTwinkleTimer = new QTimer();
        }
        if(m_ptrIconTwinkleTimer->isActive())
        {
            m_ptrIconTwinkleTimer->stop();
        }
        m_ptrIconTwinkleTimer->setInterval(TwinkleTime);
        m_ptrIconTwinkleTimer->start();
    }
    else
    {
        if(m_ptrIconTwinkleTimer)
        {
            m_ptrIconTwinkleTimer->stop();
        }
        m_ptrTrayIcon->setIcon(QIcon(":/LogInWidget/image/icon.png"));
    }
}

void ChatWidget::onSignalIconTwinkleTimerout()
{
    if (TrayIconState::Normal == m_iTrayState)
    {
        m_ptrTrayIcon->setIcon(QIcon(":/LogInWidget/image/icontrans.png"));
        m_iTrayState = TrayIconState::None;
    }
    else if (TrayIconState::None == m_iTrayState)
    {
        m_ptrTrayIcon->setIcon(QIcon(":/LogInWidget/image/icon.png"));
        m_iTrayState = TrayIconState::Normal;
    }
    m_ptrIconTwinkleTimer->stop();
    m_ptrIconTwinkleTimer->setInterval(TwinkleTime);
    m_ptrIconTwinkleTimer->start();
}

void ChatWidget::setProfileImage(const QString& strImage)
{
    QImage image(strImage);
    QPixmap pixmap=QPixmap::fromImage(image);
    QPixmap fitpixmap = pixmap.scaled(50, 50, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    ui->profileImageButton->setIcon(QIcon(fitpixmap));
    ui->profileImageButton->setIconSize(QSize(50, 50));
    ui->profileImageButton->setFlat(true);
}

void ChatWidget::onSignalChatWidOpenProfileImagePreview(const int id)
{
    if (nullptr == m_ptrProfileImagePreviewWid)
    {
        m_ptrProfileImagePreviewWid = new ProfileImagePreview();
    }
    m_ptrProfileImagePreviewWid->setImagePath(kDefaultProfileImageWidget, id);
    m_ptrProfileImagePreviewWid->show();
}


void ChatWidget::initFriendList()
{
    for (const auto& item : PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec())
    {
        QString imagePath = "";
        if ("" == item.m_strImagePath)
        {
            imagePath = kDefaultProfileImage;
        }
        else
        {
            imagePath = QString::fromStdString(item.m_strImagePath);
        }
        QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, imagePath),
                                  Q_ARG(QVariant, QString::fromStdString(item.m_strName)),
                                  Q_ARG(QVariant, QString::fromStdString(item.m_strId)),
                                  Q_ARG(QVariant, QString::fromStdString(item.m_strFirstChacter)));
    }
    initLastChatList();
    initAllChatWid();
}

void ChatWidget::initChatMessageWidAcordId(const MyLastChatFriendInfo& lastChatInfo)
{
    //创建和这个id的聊天界面，也就是加载qml，并保存在stackedwidget中
    const auto tmpWid = new MyChatMessageQuickWid();
    tmpWid->setSource(QUrl(QStringLiteral("qrc:/QML/QML/chatMessage.qml")));
    tmpWid->setRootObj();
    tmpWid->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
    tmpWid->setClearColor(QColor(238, 238, 238));


    //根据从数据库的到的id去找详细信息
    const int posInVecWithC = PublicDataManager::get_mutable_instance().getMyUsetInfoMap()[lastChatInfo.m_strId];
    const auto friendInfo = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec()[posInVecWithC];

    const QString strId = QString::fromStdString(friendInfo.m_strId);
    const QString strName = QString::fromStdString(friendInfo.m_strName);

    //保存一下这个页面所对应的好友的id
    tmpWid->SetUserId(strId);
    tmpWid->setUserName(strName);
    //把用户id设置进qml
    QMetaObject::invokeMethod(tmpWid->getRootObj(), "setId", Q_ARG(QVariant, strId));

    //聊天界面上划要求更新聊天界面内的内容
    connect(tmpWid->getRootObj(), SIGNAL(signalUpdateChatModel(QString)), this,
            SLOT(onSignalUpdateChatMessage(QString)));
    //聊天界面中点击了头像
    connect(tmpWid->getRootObj(), SIGNAL(signalProfileImageClicked(QString)), this, SLOT(onSignalChatWidProfileImageClicked(QString)));

    //数据库中没有和这个人的聊天记录，就先创建一个表
    if (!DataBaseDelegate::Instance()->isTableExist("chatrecord" + strId))
    {
        DataBaseDelegate::Instance()->createUserChatTable(strId);
    }

    //设置和这位好友的聊天记录totalcount
    tmpWid->setTotalRecordCount(DataBaseDelegate::Instance()->getChatRecordCountFromDB(strId));
    //获取聊天记录
    const std::vector<MyChatMessageInfo> vecMyChatMessageInfo = ChatWidgetManager::Instance()->getChatMessageAcordIdAtInit(
        lastChatInfo.m_strId);

    //存储一下现在页面中加载了的聊天记录数量，以便刷新时知道从哪个位置再加载
    tmpWid->addCurrentRecordCount(static_cast<int>(vecMyChatMessageInfo.size()));

    ui->chatStackedWidget->addWidget(tmpWid);
    //保存一下这个id对应的widget在stackedwidget中的位置，以便跳转使用
    ui->chatStackedWidget->insertToMap(lastChatInfo.m_strId.toInt(), tmpWid);

    //把聊天记录加载进去
    for (const auto& item : vecMyChatMessageInfo)
    {
        QString strIds = "";
        if(item.m_bIsSelf)
        {
            strIds = m_strUserId;
        }
        else
        {
            strIds = lastChatInfo.m_strId;
        }
        QMetaObject::invokeMethod(tmpWid->getRootObj(), "insertMessageModel", Q_ARG(QVariant, (item.m_strName)),
                                  Q_ARG(QVariant, (item.m_strMessage)), Q_ARG(QVariant, item.m_bIsSelf),
                                  Q_ARG(QVariant, (item.m_strName.mid(0, 1))), Q_ARG(QVariant, strIds),Q_ARG(QVariant,QString(kDefaultProfileImage)));
        tmpWid->setInitial(item.m_strName.mid(0, 1));
    }

    //滚动到底部
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
    if(m_ptrIconTwinkleTimer!= nullptr && m_ptrIconTwinkleTimer->isActive())
    {
        m_ptrIconTwinkleTimer->stop();
    }
    delete m_ptrIconTwinkleTimer;
    m_ptrNullMessageTimer = nullptr;
    if (m_ptrProfileImagePreviewWid)
    {
        m_ptrProfileImagePreviewWid->hide();
        delete m_ptrProfileImagePreviewWid;
    }
    m_ptrProfileImagePreviewWid = nullptr;
    delete m_ptrTCPThread;
    m_ptrTCPThread = nullptr;
}
