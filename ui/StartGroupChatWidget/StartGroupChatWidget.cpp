#include "StartGroupChatWidget.h"
#include "module/HTTP/ChunkedUploadFile/ChunkedUploadFile.h"

using namespace module;

namespace wechat
{
    void sendStartGroupChatJsonData(std::shared_ptr<protocol::JsonBaseData> jsondataPtr, const std::string& imagePath)
    {
        if ("" == imagePath)
        {
            return;
        }
        std::shared_ptr<protocol::StartGroupJsonData> ptr = std::dynamic_pointer_cast<protocol::StartGroupJsonData>(jsondataPtr);
        ptr->m_strImagePathInServer = imagePath;
        TCPOperateInterface::get_mutable_instance().sendMessageExternalInterface(ptr->generateJson());
    }

    StartGroupChatWidget::StartGroupChatWidget(QWidget* parent)
        : QWidget(parent)
    {
        ui.setupUi(this);
        initUI();
        initData();
        initConnect();
    }

    StartGroupChatWidget::~StartGroupChatWidget()
    {}

    void StartGroupChatWidget::setModelData(std::vector<MyFriendInfoForStartGroupChat>& friendInfo)
    {
        if (m_ptrChooseFriendModel)
        {
            m_ptrChooseFriendModel->setData(friendInfo);
        }
    }

    void StartGroupChatWidget::onSignalFinishClicked()
    {
        hide();
        //从model中获取所有的id，暂时不考虑群主概念
        if (m_ptrAddFriendModel)
        {
            std::vector<std::string> vecId = m_ptrAddFriendModel->getAllId();
            //只有选中的数量大于1才发起群聊，等于1那就打开和这个人的对话
            m_ptrAddFriendModel->clearModel();

            if (vecId.size() <= 1)
            {
                return;
            }

            //根据几个好友的头像绘制九宫格头像
            std::vector<std::string> vecFriendImagePath;
            //vecFriendImagePath.push_back(PublicDataManager::get_mutable_instance().getImagePath().toStdString());
            PublicDataManager::get_mutable_instance().getFriendImagePathVec(vecId, vecFriendImagePath);

            //根据id生成群头像
            auto image = Base::image::generateGridImage(vecFriendImagePath.size(), vecFriendImagePath);
            //获取当前时间戳，qt接口
            std::string strTimeStamp = Base::timeToString("%Y%m%d%H%M%S");
            //生成图片保存路径
            std::string strImagePath = strTimeStamp + ".png";

            image.save(QString::fromStdString(strImagePath));

            //TODO图片等待使用http传送，并返回一个保存地址，地址给到下边结构体

            auto startGroupChat = std::make_shared<protocol::StartGroupJsonData>();
            startGroupChat->m_strStarterId = PublicDataManager::get_mutable_instance().getMyId().toStdString();
            startGroupChat->m_strGroupName = m_ptrAddFriendModel->getGroupName();
            startGroupChat->m_vecGroupChat = vecId;
            auto parentPtr = dynamic_pointer_cast<protocol::JsonBaseData>(startGroupChat);

            TaskForResponse task = std::bind(&sendStartGroupChatJsonData, parentPtr, std::placeholders::_2);

            auto uploadPtr = std::make_shared<module::FileUploader>();
            ThreadPool::get_mutable_instance().submit(std::bind(&FileUploader::uploadFileWithJson, uploadPtr, fileUploadUrl, strImagePath.c_str(), task));

            //TCPThread::get_mutable_instance().sendImage(ba, startGroupChat.generateJson().c_str());
        }
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
        if (m_ptrAddFriendModel)
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
        ui.cancelPushButton->setFixedSize({ 130,40 });
    }

    void StartGroupChatWidget::initData()
    {
        m_ptrChooseFriendModel = new ChooseFriendModel();
        m_ptrChooseFriendDelegate = new ChooseFriendDelegate();
        m_ptrChooseFriendSortModel = new ChooseFriendSortModel();
        m_ptrChooseFriendSortModel->setSourceModel(m_ptrChooseFriendModel);
        ui.chooseListView->setModel(m_ptrChooseFriendSortModel);
        ui.chooseListView->setItemDelegate(m_ptrChooseFriendDelegate);

        m_ptrAddFriendModel = new AddedFriendModel();
        m_ptrAddFriendDelegate = new AddedFriendDelegate();
        ui.addedListView->setModel(m_ptrAddFriendModel);
        ui.addedListView->setItemDelegate(m_ptrAddFriendDelegate);
    }

    void StartGroupChatWidget::initConnect()
    {
        connect(ui.finishPushButton, &QPushButton::clicked, this, &StartGroupChatWidget::onSignalFinishClicked);
        connect(ui.cancelPushButton, &QPushButton::clicked, this, &StartGroupChatWidget::onSignalCancelClicked);
        connect(m_ptrChooseFriendDelegate, &ChooseFriendDelegate::signalClickChooseFriendBox, this, &StartGroupChatWidget::onSignalUpdateSelectedState);
        connect(ui.searchLineEdit, &QLineEdit::textChanged, this, &StartGroupChatWidget::onSignalSearchChanged);
        connect(m_ptrAddFriendDelegate, &AddedFriendDelegate::signalCloseFromAddedList, this, &StartGroupChatWidget::onSignalAddedDeleted);
    }
}