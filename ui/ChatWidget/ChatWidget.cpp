﻿#include "ChatWidget.h"
#include "ui_ChatWidget.h"
#include "module/Log/Log.h"
#include "ui/PublicGUI/MyTextEdit/MyTextEdit.h"
#include "ui/PublicGUI/MyLineEdit/MyLineEdit.h"
#include "module/FileManager/FileManager.h"
#include "module/PublicFunction/PublicFunction.h"
#include "module/PublicDataManager/PublicDataManager.h"
#include "module/ChatWidgetManager/ChatWidgetManager.h"
#include "protocol/ChatMessageJsonData/SingleChatMessageJsonData.h"
#include "protocol/GetFriendListReplyData/GetFriendListReplyData.h"
#include "protocol/getProfileImageJsonData/getProfileImageJsonData.h"

#include <QDir>
#include <ctime>
#include <QSoundEffect>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QPushButton>
#include <QQmlContext>
#include <QQmlEngine>

const QString ChatRecordTable = "chatrecord";

using namespace module;
namespace wechat
{
    QObject* getDataBaseSingleInstance(QQmlEngine* engine, QJSEngine* scriptEngine)
    {
        auto instance = database::DatabaseOperateForQml::instance().get();
        QQmlEngine::setObjectOwnership(instance, QQmlEngine::CppOwnership);
        return instance;
    }

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
        //初始化文件夹，没有就创建
        ChatWidgetManager::Instance()->initDirAndFile();
        ChatWidgetManager::Instance()->initDBOperateThread();
        database::DataBaseOperate::get_mutable_instance().init();
        QString imagePath = kDefaultProfileImage;
        database::ProfilePictureDatabase profilePictureDatabase;
        profilePictureDatabase.queryProfilePicturePath(m_strUserId, imagePath);
        PublicDataManager::get_mutable_instance().setImagePath(imagePath);
        initData();
        initUi();
        ChatWidgetManager::Instance()->setQMLRootPtr(m_ptrAddFriendQMLRoot, m_ptrFriendListQMLRoot, m_ptrLastChatQMLRoot);
        initConnect();
        ChatWidgetManager::Instance()->notifyServerOnline();
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
            Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strId)),
            Q_ARG(QVariant, false));
        if (!friendInfo.m_bIsGroupChat)
        {
            auto tmp = MyLastChatFriendInfo();
            tmp.m_strId = QString::fromStdString(friendInfo.m_strId);
            tmp.m_strName = QString::fromStdString(friendInfo.m_strName);
            QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "switchToItemAndChangeColor", Q_ARG(QVariant, tmp.m_strId));
            initChatMessageWidAcordId(tmp);
            //chatstackwidget也要跳转到新来的这个界面
            ui->chatStackedWidget->SwitchToChatPage(ChatWid);
            database::UserChatDatabase userChatDatabase(friendInfo.m_strId.c_str(), "");
            //新添加的好友，也创建一条聊天记录表
            userChatDatabase.createChatTable();
        }
        else
        {
            auto tmp = MyLastChatGroupInfo();
            tmp.m_strGroupId = friendInfo.m_strId.c_str();
            tmp.m_strGroupName = friendInfo.m_strName.c_str();
            initGroupChatMessageWidAcordId(tmp);
            ui->chatStackedWidget->SwitchToChatPage(ChatWid);
            database::GroupChatDatabase groupChatDatabase(friendInfo.m_strId.c_str());
            //新添加的好友，也创建一条聊天记录表
            groupChatDatabase.createTable();
        }
    }

    void ChatWidget::changeEvent(QEvent* event)
    {
        if (this->windowState() == Qt::WindowMinimized)
        {
            ui->lineEdit->clearFocus();
            convertFromSearchWid2LastChatWid();
        }
    }

    void ChatWidget::onSignalSearchTextChange(const QString& searchMsg)
    {
        //把输入的字符串中的中文转换为拼音
        QString strPinYin = "";
        strPinYin = Base::PinYin::convertToPinYin(searchMsg);
        std::map<int, QVariant> tmpMap;
        tmpMap[(int)UserRoleDefine::FriendListName] = QVariant(strPinYin);
        m_ptrSearchFriendSortModel->setFilterMap(tmpMap);
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
            QString id = "";
            std::string timeStamp = Base::timeToString("%F-%T");

            if (CurrentChatWidgetState::Single == PublicDataManager::get_mutable_instance().getCurrentChatWidgetState())
            {
                //获取界面对应的id
                id = m_ptrChatMessageWid->GetUserId();

                ChatWidgetManager::Instance()->handleSingleChatMessage(lineEditMessage, id, timeStamp, m_ptrChatMessageWid);
            }
            else if (CurrentChatWidgetState::Group == PublicDataManager::get_mutable_instance().getCurrentChatWidgetState())
            {
                id = m_ptrChatMessageWid->getGroupId();
            }

            //界面相关的操作
            QString imagePath = PublicDataManager::get_mutable_instance().getImagePath();
            if (imagePath.mid(0, 4) != "qrc:")
            {
                imagePath = "file:///" + imagePath;
            }

            //更新到界面中
            QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, m_strUserName),
                Q_ARG(QVariant, lineEditMessage), Q_ARG(QVariant, true),
                Q_ARG(QVariant, m_strUserName.mid(0, 1)), Q_ARG(QVariant, m_strUserId), Q_ARG(QVariant, imagePath));
            QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "updateLastChatMsg", Q_ARG(QVariant, lineEditMessage), Q_ARG(QVariant, id));
            //把消息更新到界面中
            ui->textEdit->clear();
            m_ptrChatMessageWid->addTotalAndCurrentRecordCount(1);
            QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "findPosInModelAndMove2Top", Q_ARG(QVariant, id));
            ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
            //因为位置改变了，要重新排列顺序
            //看位置是否发生了变化再决定是否更新数据库
        }
    }

    void ChatWidget::onSignalLastChatItemClicked(const QString strId, const QString strName, bool bIsGroupChat)
    {
        //如果就是这个id，则不做处理
        if ((bIsGroupChat == (PublicDataManager::get_mutable_instance().getCurrentChatWidgetState() == CurrentChatWidgetState::Group)) && (strId == PublicDataManager::get_mutable_instance().getCurrentChatWidgetUserInfo().userId.c_str() || strId == PublicDataManager::get_mutable_instance().getCurrentGroupChatInfo().groupId.c_str()))
        {
            return;
        }
        else
        {
            if (!bIsGroupChat)
            {
                PublicDataManager::get_mutable_instance().setCurrentChatWidgetState(CurrentChatWidgetState::Single);
                //现在的聊天页面换成这个id
                //TODO 这里的聊天记录数量不一定是10
                PublicDataManager::get_mutable_instance().setCurrentChatWidgetUserInfo(CurrentChatWidgetUserInfo(10, strName.toStdString()));
                initChatMessageWidAcordId(MyLastChatFriendInfo(strName, strId));
                ui->chatStackedWidget->SwitchToChatPage(ChatWid);
                ui->nickNameLabel->setText(strName);
                onSignalHideRedRectangleInLastChat(strId);
            }
        }
    }

    //点击好友列表之后，要把添加到lastchat中，并且改变lastchat中的颜色
    void ChatWidget::onSignalFriendListItemClicked(QString strId, QString name, bool bIsGroupChat)
    {
        //左侧列表界面设为lastchat
        ui->friendStackedWidget->setCurrentIndex(LastChatWidget);

        //如果不存在才添加，如果不是这个界面，还要切换
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
            if (imagePath.mid(0, 4) != "qrc:")
            {
                imagePath = QString("file:///") + imagePath;
            }
        }
        PublicDataManager::get_mutable_instance().setCurrentChatWidgetState(CurrentChatWidgetState::Single);
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "judgeAndInsertToModel", Q_ARG(QVariant, imagePath),
            Q_ARG(QVariant, name), Q_ARG(QVariant, strId), Q_ARG(QVariant, false));

        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "switchToItemAndChangeColor", Q_ARG(QVariant, strId));

        //插入到上次聊天的数据库中
        //DataBaseDelegate::Instance()->insertLastChat(strId);
        if (!PublicDataManager::get_mutable_instance().isIdExistInLastChatList(strId))
        {
            database::LastChatDatabase lastChatDatabase;
            lastChatDatabase.insertLastChat(strId, false);
            PublicDataManager::get_mutable_instance().insertLastChatList({ name, strId });
        }

        //如果当前聊天页面不是这个人
        if (strId != PublicDataManager::get_mutable_instance().getCurrentChatWidgetUserInfo().userId.c_str())
        {
            //现在的聊天页面换成这个id
            PublicDataManager::get_mutable_instance().setCurrentChatWidgetUserInfo(CurrentChatWidgetUserInfo(10, name.toStdString()));
            initChatMessageWidAcordId(MyLastChatFriendInfo(name, strId));
            ui->chatStackedWidget->SwitchToChatPage(ChatWid);
        }

        //去除掉红点
        onSignalHideRedRectangleInLastChat(strId);
        ui->nickNameLabel->setText(name);
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
        if (!m_bLastChatInitFinished)
        {
            std::unique_lock<std::mutex> lck(m_mutex);
            m_con.wait(lck, [this]() {return m_bLastChatInitFinished == true; });
        }
        QApplication::alert(this);
        //QSound::play(":/res/sound/message.wav");
        const protocol::SingleChatMessageJsonData singleChatData(chatMessage.toStdString());
        const QString sendId = QString::fromStdString(singleChatData.m_strSendUserId);
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setLastChatRecord", Q_ARG(QVariant, chatMessage), Q_ARG(QVariant, sendId));
        //存储在数据库中
        auto tablename = "chatrecord" + singleChatData.m_strSendUserId;

        database::UserChatDatabase userChatDatabase(sendId.toStdString().c_str(), "");
        const int totalCnt = userChatDatabase.queryChatRecordCountFromDB(sendId);
        if (database::DataBaseOperate::get_const_instance().isTableExist(QString::fromStdString(tablename)))
        {
            //添加到数据库
            userChatDatabase.insertChatRecoed(totalCnt, QString::fromStdString(singleChatData.m_strMessage), QString::fromStdString(singleChatData.m_strTime), false, QString::fromStdString(singleChatData.m_strSendName));
        }
        else
        {
            //没有就创建这个表
            userChatDatabase.createChatTable();
            //添加到数据库
            userChatDatabase.insertChatRecoed(totalCnt, QString::fromStdString(singleChatData.m_strMessage), QString::fromStdString(singleChatData.m_strTime), false, QString::fromStdString(singleChatData.m_strSendName));
        }

        //如果当前的chat页面不是来消息的这个人，就开始闪烁，并显示红点，是的话就直接更新到界面
        if ("" == m_ptrChatMessageWid->GetUserId() || PublicDataManager::get_mutable_instance().getCurrentChatWidgetUserInfo().userId.c_str() != m_ptrChatMessageWid->GetUserId())
        {
            PublicDataManager::get_mutable_instance().setUnreadMsg(sendId, 1);
            QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setRedMsgAndShow", Q_ARG(QVariant, sendId),
                Q_ARG(QVariant, QString::number(PublicDataManager::get_mutable_instance().getUnreadMsgCnt(sendId))));
            //onSignalSetRedRectangleShow(s22.c_str());
            onSignalTrayIconTwinkle(true);
        }
        else
        {
            QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "appendMessageModel", Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strSendName)),
                Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strMessage)),
                Q_ARG(QVariant, false), Q_ARG(QVariant, QString::fromStdString(singleChatData.m_strSendName).mid(0, 1)),
                Q_ARG(QVariant, atoi(singleChatData.m_strRecvUserId.c_str())), Q_ARG(QVariant, ""));
            m_ptrChatMessageWid->addTotalAndCurrentRecordCount(1);
        }
        //更新上次聊天qml界面中上次聊天信息
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "updateFriendLastChatMsg", Q_ARG(QVariant, singleChatData.m_strMessage.c_str()));
    }

    void ChatWidget::onSignalSearchTextLoseFocus(bool isFocus)
    {
        if (isFocus)
        {
            //如果获得了焦点就切换到好友列表界面，还需要设置一下过滤规则，这个根据输入的内容确定
            ui->friendStackedWidget->setCurrentIndex(SearchFriendWidget);
            //颜色修改一下
            QPalette pal = ui->widget_3->palette();
            pal.setColor(QPalette::Window, Qt::white);
            ui->widget_3->setPalette(pal);
        }
        //else
        //{
        //	//颜色修改回去
        //	QPalette pal = ui->widget_3->palette();
        //	pal.setColor(QPalette::Window, QColor(238, 238, 238));
        //	ui->widget_3->setPalette(pal);
        //	if (ui->lineEdit->text().isEmpty())
        //	{
        //		ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
        //	}
        //}
    }

    void ChatWidget::onSignalChatBtn()
    {
        ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
        ui->chatStackedWidget->SwitchToChatPage(ChatWid);
    }

    void ChatWidget::onSignalFriendListBtn()
    {
        ui->friendStackedWidget->setCurrentIndex(FriendListWidget);
        ui->chatStackedWidget->SwitchToChatPage(EmptyWid);
    }

    void ChatWidget::onSignalAddFriendBtn()
    {
        //ui->chatStackedWidget->SwitchToChatPage(AddFriendWid);
        //如果添加好友界面为空，就创建一个
        if (nullptr == m_ptrAddFriendWid)
        {
            m_ptrAddFriendWid = new AddFriendWidget();
            m_ptrAddFriendWid->show();
        }
        else
        {
            m_ptrAddFriendWid->show();
        }
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
        //然后根据wid得到现在还有多少记录可以加载
        //用总的记录减去已加载的数量
        int curCount = m_ptrChatMessageWid->getRecordCount();
        int canLoadCount = m_ptrChatMessageWid->getTotalRecordCount() - m_ptrChatMessageWid->getRecordCount();
        //聊天记录如果没有就不更新，有且大于10条就更新10 条
        canLoadCount = canLoadCount > 10 ? 10 : canLoadCount;
        std::vector<MyChatMessageInfo> vecMyChatMessageInfo;

        if (0 == canLoadCount)
        {
            return;
        }
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "setBusyIndicatorStateFlag", Q_ARG(QVariant, true));
        //查询聊天记录的起始位置是聊天页面当前的数量,加载的总数量是canLoadCount
        database::UserChatDatabase userChatDatabase(id.toStdString().c_str(), "");
        userChatDatabase.queryCertainCountChatRecordAcodId(id, vecMyChatMessageInfo, canLoadCount,
            m_ptrChatMessageWid->getRecordCount());
        //存储一下现在页面中加载了的聊天记录数量，以便刷新时知道从哪个位置再加载
        m_ptrChatMessageWid->addCurrentRecordCount(vecMyChatMessageInfo.size());

        database::ProfilePictureDatabase profilePictureDatabase;
        //把聊天记录加载进去
        for (const auto& item : vecMyChatMessageInfo)
        {
            QString strId = "";
            QString imagePath = "";
            if (item.m_bIsSelf)
            {
                strId = m_strUserId;
                imagePath = PublicDataManager::get_mutable_instance().getImagePath();
                if (imagePath.mid(0, 4) != "qrc:")
                {
                    imagePath = "file:///" + imagePath;
                }
            }
            else
            {
                strId = id;
                imagePath = kDefaultProfileImage;

                profilePictureDatabase.queryProfilePicturePath(id, imagePath);
                if (imagePath.mid(0, 4) != "qrc:")
                {
                    imagePath = "file:///" + imagePath;
                }
            }
            QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "insertMessageModel", Q_ARG(QVariant, (item.m_strName)),
                Q_ARG(QVariant, (item.m_strMessage)), Q_ARG(QVariant, item.m_bIsSelf),
                Q_ARG(QVariant, (item.m_strName.mid(0, 1))), Q_ARG(QVariant, strId), Q_ARG(QVariant, imagePath));
        }
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "setBusyIndicatorStateFlag", Q_ARG(QVariant, false));
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "scrollToPosition", Q_ARG(QVariant, canLoadCount - 1));
    }

    //好友的头像修改了以后就更新一下lastchat，Friendlist，并查看是不是和他聊天呢，是就把chatwidget也更新一下
    void ChatWidget::onSignalFriendProfileImageChanged(const QString& id, const QString& imagePath) const
    {
        //publicmanager中的friendvec中的头像路径也要更新
        auto qmlImagePath = "file:///" + imagePath;
        PublicDataManager::get_mutable_instance().updateProfileImagePathOfFriendInfoVec(id, imagePath);
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "updateFriendImage", Q_ARG(QVariant, id), Q_ARG(QVariant, qmlImagePath));
        QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "updateFriendImage", Q_ARG(QVariant, id), Q_ARG(QVariant, qmlImagePath));
        if (PublicDataManager::get_mutable_instance().getCurrentChatWidgetUserInfo().userId.c_str() == id)
        {
            //更新一下界面里的内容
            QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "updateFriendImage", Q_ARG(QVariant, id), Q_ARG(QVariant, qmlImagePath));
        }
    }

    void ChatWidget::onSignalAddFriendProfileImage(const QString& id, const QString& timeStamp, const QString& imagePath) const
    {
        database::ProfilePictureDatabase profilePictureDatabase;
        m_ptrAddFriendWid->updateModelImagePath(id, imagePath);
        profilePictureDatabase.insertProfilePathAndTimestamp(id, imagePath, timeStamp);
    }

    void ChatWidget::onSignalStartGroupChatClicked()
    {
        if (!m_ptrStartGroupChatWidget)
        {
            m_ptrStartGroupChatWidget = new StartGroupChatWidget();
        }
        m_ptrStartGroupChatWidget->setModelData(PublicDataManager::get_mutable_instance().getFriendInfoForStartGroupChat());
        m_ptrStartGroupChatWidget->show();
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

        qmlRegisterSingletonType<database::DatabaseOperateForQml>("CPPService", 1, 0, "DataBaseOperate", getDataBaseSingleInstance);

        //把空的界面加入到stackedwid,有时会用到空白界面
        m_ptrEmptyWid = new MyChatMessageQuickWid();
        ui->chatStackedWidget->addWidget(m_ptrEmptyWid);
        ui->chatStackedWidget->insertToMap(EmptyWid, m_ptrEmptyWid);
        ui->chatStackedWidget->SwitchToChatPage(EmptyWid);

        //把实际的聊天界面添加到stackwid中，和每个人聊天都用这个wid
        m_ptrChatMessageWid = new MyChatMessageQuickWid();
        m_ptrChatMessageWid->setSource(QUrl(("qrc:/QML/QML/chatMessage.qml")));
        m_ptrChatMessageWid->setRootObj();
        m_ptrChatMessageWid->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
        m_ptrChatMessageWid->setClearColor(QColor(238, 238, 238));
        ui->chatStackedWidget->addWidget(m_ptrChatMessageWid);
        ui->chatStackedWidget->insertToMap(ChatWid, m_ptrChatMessageWid);

        //上次聊天列表设置qml文件
        m_ptrLastChatWidget->setSource(QUrl("qrc:/QML/QML/LastChatList.qml"));
        m_ptrLastChatWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
        //得到根对象
        m_ptrLastChatQMLRoot = reinterpret_cast<QObject*>(m_ptrLastChatWidget->rootObject());

        m_ptrFriendListWidget->setSource(QUrl("qrc:/QML/QML/friendList.qml"));
        m_ptrFriendListWidget->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);
        m_ptrFriendListQMLRoot = reinterpret_cast<QObject*>(m_ptrFriendListWidget->rootObject());

        m_ptrSearchFriendModel = new MyFriendListModel;
        m_ptrSearchFriendSortModel = new MyFriendListSortModel;
        m_ptrSearchFriendSortModel->setSourceModel(m_ptrSearchFriendModel);
        m_ptrSearchFriendList->rootContext()->setContextProperty("searchFriendModel", m_ptrSearchFriendSortModel);
        m_ptrSearchFriendList->setSource(QUrl("qrc:/QML/QML/searchFriend.qml"));
        m_ptrSearchFriendList->setResizeMode(QQuickWidget::ResizeMode::SizeRootObjectToView);

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

        initAddFriendWid();

        setProfileImage(PublicDataManager::get_mutable_instance().getImagePath());
    }

    void ChatWidget::initConnect()
    {
        //ui界面中的部件被点击
        connect(ui->pushButton, &QPushButton::clicked, this, &ChatWidget::onSignalSendMessage);
        //QML页面通知要更新lastchat了
        connect(m_ptrLastChatQMLRoot, SIGNAL(signalNeedUpdateLastChat()), this, SLOT(onSignalNeedUpdateLastChat()));

        //QML页面上次聊天界面用户被点击
        connect(m_ptrLastChatQMLRoot, SIGNAL(signalFriendListClicked(QString, QString, bool)), this,
            SLOT(onSignalLastChatItemClicked(QString, QString, bool)));
        //QML页面好友列表被点击
        connect(m_ptrFriendListQMLRoot, SIGNAL(signalFriendListClicked(QString, QString, bool)), this,
            SLOT(onSignalFriendListItemClicked(QString, QString, bool)));

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

        //收到好友消息列表后，由manager去处理数据
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalRecvFriendListMessage, this,
            &ChatWidget::onSignalRecvFriendList, Qt::QueuedConnection);
        //收到服务端好友聊天消息
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalRecvSingleChatMessage, this,
            &ChatWidget::onSignalSingleChatMessage, Qt::QueuedConnection);
        //收到服务端好友添加请求
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalNewFriendRequest, ChatWidgetManager::Instance().get(),
            &ChatWidgetManager::onSignalNewFriendRequest, Qt::QueuedConnection);
        //收到服务端同意好友添加
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalBecomeFriendNotify, ChatWidgetManager::Instance().get(),
            &ChatWidgetManager::onSignalBecomeFriend, Qt::QueuedConnection);
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalStartGroupChatReply, ChatWidgetManager::Instance().get(), &ChatWidgetManager::onSignalStartGroupChatReply, Qt::QueuedConnection);

        //侧边栏三个按钮的响应
        connect(ui->chatPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalChatBtn);
        connect(ui->friendListPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalFriendListBtn);
        connect(ui->addFriendPushButton, &QPushButton::clicked, this, &ChatWidget::onSignalAddFriendBtn);
        connect(ui->profileImageButton, &QPushButton::clicked, this, &ChatWidget::onSignalSideBarProfileImageBtn);

        //点击发起群聊
        connect(ui->startGroupChatBtn, &QPushButton::clicked, this, &ChatWidget::onSignalStartGroupChatClicked);
        //manager收到了一个群聊，通知到lastchat界面
        connect(ChatWidgetManager::Instance().get(), &ChatWidgetManager::signalAddGroupChat2LastChat, this, &ChatWidget::onSignalAdd2LastChat);

        //搜索框获取焦点响应
        connect(ui->lineEdit, &MyLineEdit::signalIsFocus, this, &ChatWidget::onSignalSearchTextLoseFocus);
        //搜索框内容发生变化响应
        connect(ui->lineEdit, &MyLineEdit::textChanged, this, &ChatWidget::onSignalSearchTextChange);
        //对话框或文本输入框获取焦点后的响应
        connect(ui->textEdit, &MyTextEdit::signalTextEditIsFocus, this, &ChatWidget::onSignalTextOrChatFocusIn);
        connect(m_ptrEmptyWid, &MyChatMessageQuickWid::signalFocusChanged, this, &ChatWidget::onSignalTextOrChatFocusIn);
        connect(m_ptrChatMessageWid, &MyChatMessageQuickWid::signalFocusChanged, this, &ChatWidget::onSignalTextOrChatFocusIn);

        //聊天界面上划要求更新聊天界面内的内容
        connect(m_ptrChatMessageWid->getRootObj(), SIGNAL(signalUpdateChatModel(QString)), this,
            SLOT(onSignalUpdateChatMessage(QString)));
        //聊天界面中点击了头像
        connect(m_ptrChatMessageWid->getRootObj(), SIGNAL(signalProfileImageClicked(QString)), this, SLOT(onSignalChatWidProfileImageClicked(QString)));
        //搜索好友界面点击了头像
        connect(reinterpret_cast<QObject*>(m_ptrSearchFriendList->rootObject()), SIGNAL(signalSearchFriendListClicked(QString, QString)), this, SLOT(onSignalSearchFriendProfileImageClicked(QString, QString)));

        //自己的头像换了
        connect(m_ptrProfileImagePreviewWid, &ProfileImagePreview::signalProfileImageChanged, this, &ChatWidget::onSignalProfileImageChanged);
        //好友头像更换后的处理
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalProfileImageChanged, this, &ChatWidget::onSignalFriendProfileImageChanged, Qt::QueuedConnection);
        //收到添加好友人的头像
        connect(&TCPOperateInterface::get_mutable_instance(), &TCPOperateInterface::signalAddFriendProfileImage, this, &ChatWidget::onSignalAddFriendProfileImage, Qt::QueuedConnection);
    }

    //初始化一些指针和需要的数据
    void ChatWidget::initData()
    {
        m_ptrFriendListWidget = new QQuickWidget();
        //m_ptrNewFriendAndAreadyAddWidget = new QQuickWidget();
        m_ptrLastChatWidget = new QQuickWidget();
        m_ptrSearchFriendList = new QQuickWidget();

        m_ptrIconTwinkleTimer = new QTimer();
        m_ptrProfileImagePreviewWid = new ProfileImagePreview();
        m_ptrAddFriendWid = new AddFriendWidget();

        m_ptrStartGroupChatWidget = new StartGroupChatWidget();

        //先去从缓存的数据库中获取到相关的数据并更新到实际的数据库，然后在执行相关操作
        auto dataBase = QSqlDatabase::addDatabase("QSQLITE", "sqlite2");
        connectToBackupDB(dataBase);
        std::vector<std::pair<QString, bool>> tmpOrder;
        getLastChatFromBackup(tmpOrder, dataBase);
        disConnectBackupDB(dataBase);
        database::LastChatDatabase lastChatDatabase;
        //清空上次聊天数据库中
        lastChatDatabase.clearLastChat();
        //写入到自己的数据库中
        lastChatDatabase.insertLastChat(tmpOrder);

        //这时候主线程对备份数据库操作完成了，子线程可以连接了
        ChatWidgetManager::Instance()->initDBThreadConnect();
        ChatWidgetManager::Instance()->getLastChatListFromDB(PublicDataManager::get_mutable_instance().getMyLastChatFriendInfoVec());
    }

    void ChatWidget::getLastChatFromBackup(std::vector<std::pair<QString, bool>>& tmpOrder, QSqlDatabase& db)
    {
        const QString str = "select * from lastchatlist" + m_strUserId;
        QSqlQuery query(db);
        if (!query.exec(str))
        {
            _LOG(Logcxx::Level::ERRORS, "getLastChatFromBackup error");
        }
        QSqlRecord record = query.record();
        while (query.next())
        {
            auto tmp = MyLastChatFriendInfo();
            record = query.record();
            tmpOrder.push_back(std::pair<QString, bool>(record.value("id").toString(), record.value("isGroupChat").toBool()));
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

    void ChatWidget::convertFromSearchWid2LastChatWid() const
    {
        if (ui->friendStackedWidget->currentWidget() == m_ptrSearchFriendList)
        {
            QPalette pal = ui->widget_3->palette();
            pal.setColor(QPalette::Window, QColor(238, 238, 238));
            ui->widget_3->setPalette(pal);
            ui->lineEdit->clear();
            ui->friendStackedWidget->setCurrentIndex(LastChatWidget);
        }
    }

    //收到服务器返回的好友列表后初始化lastchat
    void ChatWidget::initLastChatList()
    {
        for (auto& item : PublicDataManager::get_mutable_instance().getMyLastChatFriendInfoVec())
        {
            auto friendInfo = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec()[PublicDataManager::get_mutable_instance().getMyUsetInfoMap()[item.m_strId]];
            //获取上次聊天内容
            //前四个字符不是qrc:
            QString imagePath = "";
            if (friendInfo.m_strImagePath.substr(0, 4) != "qrc:")
            {
                if (Base::fileoperate::FileManager::get_mutable_instance().checkFileExist(friendInfo.m_strImagePath.c_str()))
                {
                    //如果是本地图片，需要加上file:///
                    imagePath = QString("file:///") + friendInfo.m_strImagePath.c_str();
                }
                else
                {
                    imagePath = kDefaultProfileImage;
                }
            }
            else
            {
                imagePath = friendInfo.m_strImagePath.c_str();
            }
            database::UserChatDatabase userChatDatabase(item.m_strId.toStdString().c_str(), "");
            QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "addElementToModel",
                Q_ARG(QVariant, imagePath),
                Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strName)),
                Q_ARG(QVariant, QString::fromStdString(friendInfo.m_strId)),
                Q_ARG(QVariant, friendInfo.m_bIsGroupChat),
                Q_ARG(QVariant, userChatDatabase.queryFinalChatRecord())
            );
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

    void ChatWidget::initAddFriendWid()
    {
        std::vector<MyAddFriendInfo> tmp = {};
        database::AddFriendDatabase addFriendDatabase;
        database::ProfilePictureDatabase profilePictureDatabase;
        addFriendDatabase.queryAddFriendInfoFromDB(tmp);
        std::vector<AddFriendInfo> tmp2 = {};
        for (auto& item : tmp)
        {
            AddFriendInfo tmpInfo;
            tmpInfo.isValid = item.m_bIsValid;
            tmpInfo.m_strId = item.m_strFriendId;
            tmpInfo.m_strFriendName = item.m_strFriendName;
            tmpInfo.m_strVerifyInfo = item.m_strVerifyMsg;
            QString path = "";
            profilePictureDatabase.queryProfilePicturePath(item.m_strFriendId, path);
            if (path.isEmpty())
            {
                path = kDefaultProfileImage;
            }
            tmpInfo.m_strProfileImagePath = path;
            tmp2.push_back(tmpInfo);
        }
        m_ptrAddFriendWid->setData(tmp2);
    }

    void ChatWidget::onSignalTextOrChatFocusIn(bool isFocus)
    {
        if (isFocus)
        {
            convertFromSearchWid2LastChatWid();
        }
    }

    void ChatWidget::onSignalHideRedRectangleInLastChat(const QString id)
    {
        PublicDataManager::get_mutable_instance().clearUnreadMsg(id);
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setRedMsgAndHide", Q_ARG(QVariant, id));
        onSignalTrayIconTwinkle(false);
        m_iTrayState = TrayIconState::Normal;
    }

    void ChatWidget::onSignalSetRedRectangleShow(const QString& id)const
    {
        PublicDataManager::get_mutable_instance().setUnreadMsg(id, 1);
        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "setRedMsgAndShow", Q_ARG(QVariant, id),
            Q_ARG(QVariant, QString::number(PublicDataManager::get_mutable_instance().getUnreadMsgCnt(id))));
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
        database::ProfilePictureDatabase profilePictureDatabase;
        //把这个好友的头像时间戳插入到数据库,先使用默认的头像路径
        profilePictureDatabase.insertProfilePathAndTimestamp(friendInfo.m_strId.c_str(), kDefaultProfileImage, friendInfo.m_strImageTimestamp.c_str());
    }

    void ChatWidget::onAddFriendIntoList(const MyFriendInfoWithFirstC& friendInfo)
    {
        PublicDataManager::get_mutable_instance().addFriendInfoWithC(friendInfo);
    }

    void ChatWidget::onSortFriendList()
    {
#if __cplusplus >= 202002L
        std::ranges::sort(PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().begin(), PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().end(), [&](const MyFriendInfoWithFirstC& l, const MyFriendInfoWithFirstC& r)
            {
                return l.m_strFirstChacter < r.m_strFirstChacter;
            });
#else
        std::sort(PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().begin(), PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec().end(), [](const MyFriendInfoWithFirstC& l, const MyFriendInfoWithFirstC& r) {
            return l.m_strFirstChacter < r.m_strFirstChacter;
            });
#endif
    }

    void ChatWidget::onUpdateFriendListUI() const
    {
        //先清空qml中的列表
        QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "clearModel");

        //然后再按照顺序添加进去
        for (const auto& item : PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec())
        {
            //前四个字符不是qrc:
            QString imagePath = "";
            if (item.m_strImagePath.substr(0, 4) != "qrc:")
            {
                //如果是本地图片，需要加上file:///
                imagePath = QString("file:///") + item.m_strImagePath.c_str();
            }
            else
            {
                imagePath = item.m_strImagePath.c_str();
            }
            QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, imagePath),
                Q_ARG(QVariant, QString::fromStdString(item.m_strName)),
                Q_ARG(QVariant, QString::fromStdString(item.m_strId)),
                Q_ARG(QVariant, QString::fromStdString(item.m_strFirstChacter)),
                Q_ARG(QVariant, false));
        }
    }

    void ChatWidget::onSignalNeedUpdateLastChat()const
    {
        //没有初始化上次聊天列表的时候是构造的时候添加触发的，不处理
        if (!m_bLastChatInitFinished)
        {
            return;
        }
        ChatWidgetManager::Instance()->onSignalUpdateLastChat();
    }

    void ChatWidget::onSignalChatWidProfileImageClicked(const QString id)
    {
        onSignalChatWidOpenProfileImagePreview(id.toInt());
    }

    //搜索好友界面中点击了头像
    void ChatWidget::onSignalSearchFriendProfileImageClicked(const QString id, const QString& name)
    {
        //左侧列表界面设为lastchat
        ui->friendStackedWidget->setCurrentIndex(LastChatWidget);

        //如果不存在才添加，如果不是这个界面，还要切换
        auto friendVec = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec();
        auto friendInfo = friendVec[PublicDataManager::get_mutable_instance().getMyUsetInfoMap()[id]];
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
            Q_ARG(QVariant, name), Q_ARG(QVariant, id), Q_ARG(QVariant, false));

        QMetaObject::invokeMethod(m_ptrLastChatQMLRoot, "switchToItemAndChangeColor", Q_ARG(QVariant, id));

        //插入到上次聊天的数据库中
        //DataBaseDelegate::Instance()->insertLastChat(strId);
        if (!PublicDataManager::get_mutable_instance().isIdExistInLastChatList(id))
        {
            database::LastChatDatabase lastChatDatabase;
            lastChatDatabase.insertLastChat(id, false);
            PublicDataManager::get_mutable_instance().insertLastChatList({ name, id });
        }

        //如果当前聊天页面不是这个人
        if (id != PublicDataManager::get_mutable_instance().getCurrentChatWidgetUserInfo().userId.c_str())
        {
            //现在的聊天页面换成这个id
            PublicDataManager::get_mutable_instance().setCurrentChatWidgetUserInfo(CurrentChatWidgetUserInfo(10, name.toStdString()));
            initChatMessageWidAcordId(MyLastChatFriendInfo(name, id));
            ui->chatStackedWidget->SwitchToChatPage(ChatWid);
        }

        //去除掉红点
        onSignalHideRedRectangleInLastChat(id);
        ui->nickNameLabel->setText(name);
    }

    //底部栏闪烁槽函数
    //parm[IN] bNeed true:闪烁，false:不闪烁
    void ChatWidget::onSignalTrayIconTwinkle(const bool bNeed)
    {
        if (bNeed)
        {
            if (!m_ptrIconTwinkleTimer)
            {
                m_ptrIconTwinkleTimer = new QTimer();
            }
            if (m_ptrIconTwinkleTimer->isActive())
            {
                m_ptrIconTwinkleTimer->stop();
            }
            m_ptrIconTwinkleTimer->setInterval(TwinkleTime);
            m_ptrIconTwinkleTimer->start();
        }
        else
        {
            if (m_ptrIconTwinkleTimer)
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
        QString tmpPath = strImage;
        if (strImage.mid(0, 4) == "qrc:")
        {
            tmpPath = strImage.mid(3);
        }
        QImage image(tmpPath);
        QPixmap pixmap = QPixmap::fromImage(image);
        QPixmap fitpixmap = pixmap.scaled(500, 500, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
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
        QString imagePath = kDefaultProfileImage;
        database::ProfilePictureDatabase profilePictureDatabase;
        profilePictureDatabase.queryProfilePicturePath(QString::number(id), imagePath);
        m_ptrProfileImagePreviewWid->setImagePath(imagePath, id);
        m_ptrProfileImagePreviewWid->show();
    }

    void ChatWidget::onSignalProfileImageChanged(const QString& id, const QString& imagePath)
    {
        //需要更新左上角的和聊天界面中的
        setProfileImage(imagePath);
        //如果需要更新的id是当前聊天的id或者是自己的id的时候就要更新了
        if (PublicDataManager::get_mutable_instance().getCurrentChatWidgetUserInfo().userId.c_str() == id || m_strUserId == id)
        {
            //更新一下界面里的内容
            auto qmlImagePath = "file:///" + imagePath;
            QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "updateFriendImage", Q_ARG(QVariant, id), Q_ARG(QVariant, qmlImagePath));
        }
    }

    void ChatWidget::initFriendList()
    {
        for (const auto& item : PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec())
        {
            //前四个字符不是qrc:
            QString imagePath = "";
            if (item.m_strImagePath.substr(0, 4) != "qrc:")
            {
                //如果是本地图片，需要加上file:///
                imagePath = QString("file:///") + item.m_strImagePath.c_str();
            }
            else
            {
                imagePath = item.m_strImagePath.c_str();
            }
            QMetaObject::invokeMethod(m_ptrFriendListQMLRoot, "addElementToModel", Q_ARG(QVariant, imagePath),
                Q_ARG(QVariant, QString::fromStdString(item.m_strName)),
                Q_ARG(QVariant, QString::fromStdString(item.m_strId)),
                Q_ARG(QVariant, QString::fromStdString(item.m_strFirstChacter)),
                Q_ARG(QVariant, false));
        }
        auto friendInfoVec = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec();
        m_ptrSearchFriendModel->setData(friendInfoVec);
        initLastChatList();
        m_bLastChatInitFinished = true;
        m_con.notify_one();
        //现在只有点击了才会做聊天界面的处理
        //initAllChatWid();
    }

    void ChatWidget::initChatMessageWidAcordId(const MyLastChatFriendInfo& lastChatInfo)
    {
        //根据从数据库的到的id去找详细信息
        const int posInVecWithC = PublicDataManager::get_mutable_instance().getMyUsetInfoMap()[lastChatInfo.m_strId];
        const auto friendInfo = PublicDataManager::get_mutable_instance().getMyFriendInfoWithCVec()[posInVecWithC];

        const QString strId = QString::fromStdString(friendInfo.m_strId);
        const QString strName = QString::fromStdString(friendInfo.m_strName);

        //清空上次聊天人的model
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "clearMessageModel");
        //保存一下这个页面所对应的好友的id
        m_ptrChatMessageWid->SetUserId(strId);
        m_ptrChatMessageWid->setUserName(strName);
        //把用户id设置进qml
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "setId", Q_ARG(QVariant, strId));

        database::UserChatDatabase userChatDatabase(strId.toStdString().c_str(), "");
        //数据库中没有和这个人的聊天记录，就先创建一个表
        if (!database::DataBaseOperate::get_const_instance().isTableExist("chatrecord" + strId))
        {
            userChatDatabase.createChatTable();
        }

        //设置和这位好友的聊天记录totalcount
        m_ptrChatMessageWid->setTotalRecordCount(userChatDatabase.queryChatRecordCountFromDB(strId));
        //获取聊天记录
        const std::vector<MyChatMessageInfo> vecMyChatMessageInfo = ChatWidgetManager::Instance()->getChatMessageAcordIdAtInit(
            lastChatInfo.m_strId);

        //存储一下现在页面中加载了的聊天记录数量，以便刷新时知道从哪个位置再加载
        m_ptrChatMessageWid->setRecordCount(static_cast<int>(vecMyChatMessageInfo.size()));

        //把聊天记录加载进去
        for (const auto& item : vecMyChatMessageInfo)
        {
            QString strIds = "";
            QString imagePath = "";
            if (item.m_bIsSelf)
            {
                strIds = m_strUserId;
                auto tmpPath = PublicDataManager::get_mutable_instance().getImagePath();
                if (tmpPath.mid(0, 4) != "qrc:")
                {
                    imagePath = QString("file:///") + tmpPath;
                }
                else
                {
                    imagePath = tmpPath;
                }
            }
            else
            {
                strIds = lastChatInfo.m_strId;
                if (friendInfo.m_strImagePath.substr(0, 4) != "qrc:")
                {
                    //如果是本地图片，需要加上file:///
                    imagePath = QString("file:///") + friendInfo.m_strImagePath.c_str();
                }
                else
                {
                    imagePath = friendInfo.m_strImagePath.c_str();
                }
            }
            QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "insertMessageModel", Q_ARG(QVariant, (item.m_strName)),
                Q_ARG(QVariant, (item.m_strMessage)), Q_ARG(QVariant, item.m_bIsSelf),
                Q_ARG(QVariant, (item.m_strName.mid(0, 1))), Q_ARG(QVariant, strIds), Q_ARG(QVariant, imagePath));
            m_ptrChatMessageWid->setInitial(item.m_strName.mid(0, 1));
        }

        //滚动到底部
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "scrollToEnd", Qt::DirectConnection);
    }

    void ChatWidget::initGroupChatMessageWidAcordId(const MyLastChatGroupInfo& lastChatGroupInfo)
    {
        //清空上次聊天人的model
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "clearMessageModel");
        //保存一下这个页面所对应的好友的id
        m_ptrChatMessageWid->setGroupId(lastChatGroupInfo.m_strGroupId);
        m_ptrChatMessageWid->setGroupName(lastChatGroupInfo.m_strGroupName);
        //把用户id设置进qml
        QMetaObject::invokeMethod(m_ptrChatMessageWid->getRootObj(), "setId", Q_ARG(QVariant, lastChatGroupInfo.m_strGroupId));

        database::GroupChatDatabase groupChatDatabase(lastChatGroupInfo.m_strGroupId.toStdString().c_str());
        //数据库中没有这个群聊的聊天记录，就先创建一个表
        if (!database::DataBaseOperate::get_const_instance().isTableExist("groupchat" + lastChatGroupInfo.m_strGroupId))
        {
            groupChatDatabase.createTable();
        }
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
        if (m_ptrIconTwinkleTimer != nullptr && m_ptrIconTwinkleTimer->isActive())
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

        delete m_ptrAddFriendWid;
        m_ptrAddFriendWid = nullptr;

        delete m_ptrStartGroupChatWidget;
        m_ptrStartGroupChatWidget = nullptr;
    }
}