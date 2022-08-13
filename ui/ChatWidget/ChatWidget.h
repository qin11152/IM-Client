/************************************************************************/
/*���������棬���û���¼���������Ȳ����Ľ��棬Ҳ����Ϊ��Ҫ�Ľ��� 
*/
/************************************************************************/

#pragma once

#include "../../module/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include "module/MyDefine.h"
#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QQuickWidget>
#include <QSqlDatabase>

namespace Ui 
{ 
    class ChatWidget; 
};


//��������е�stackwidget�е�ͨ�ý���id��������Ӻ���
enum ChatStackedWidgetCommonWidget
{
    AddFriendWid = 19999,
    EmptyWid
};

class ChatWidget : public QWidget
{
    Q_OBJECT

public:
    ChatWidget(QString id,QString name,QWidget *parent = Q_NULLPTR);
    virtual ~ChatWidget()override;
    ChatWidget(const ChatWidget& l) = delete;
    ChatWidget& operator=(const ChatWidget& l) = delete;
    MyChatMessageQuickWid* getChatMsgWidAcordId(QString id);

protected slots:
    //�¼�������������
    bool eventFilter(QObject* watched, QEvent* event)override;

public:
    //���ϴ������б������һ���µģ�һ����������µĺ��ѵ�ʱ�����
    void onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo);

private slots:
    //manager��������б����
    void initFriendList();
    //����id��ʼ����id��Ӧ���������
    void initChatMessageWidAcordId(const MyLastChatFriendInfo& lastChatInfo);
    //��ʼ���ϴ������б�
    void initLastChatList();
    //��ʼ���������
    void initAllChatWid();

    //�ı��������ý���
    void onSignalTextEditIsFocus(bool isFocus);
    //���������ʧȥ�����
    void onSignalSearchTextLoseFocus(bool isFocus);

    //������Ϣ��ť�������
    void onSignalSendMessage();
    //�ϴ������б������
    void onSignalLastChatItemClicked(const QString strId);
    //�����б����
    void onSignalFriendListItemClicked(QString strId,QString name);
    //�ײ����̱������
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);

    //�յ�����������Ϣ��
    void onSignalSingleChatMessage(const QString& chatMessage);
    //�յ������б���Ϣ
    void onSignalRecvFriendList(const QString& friendList);
    //�к���ͬ���˺�������
    void onSignalBecomeFriend(const MyFriendInfoWithFirstC& friendInfo);
    //������б�����Ӻ���
    void onAddFriendIntoList(const MyFriendInfoWithFirstC& friendInfo);

    //���������Ự��ť
    void onSignalChatBtn();
    //������ͨѶ¼��ť
    void onSignalFriendListBtn();
    //����������Ӻ��Ѱ�ť
    void onSignalAddFriendBtn();

    //��ĳ�����ѵ�����ҳ��Ҫ��ˢ�½��棬Ҳ����Ҫ�����������¼
    void onSignalUpdateChatMessage(const QString id);
    //���ú�ɫ��ʾ��ʾ��������ȷ������
    void onSignalSetRedRectangleShow(MyChatMessageQuickWid* ptr)const;
    //�����ȡ����ɫ��Ϣ��ʾ��
    void onSignalHideRedRectangleInLastChat(const QString id);

    //��������ĸ��Ӣ�ĶԺ����б�����
    void onSortFriendList();
    //ˢ�½����еĺ����б�
    void onUpdateFriendListUI()const;
    //QML����֪ͨ�ϴ����������Ҫд�����ݿ�
    void onSignalNeedUpdateLastChat()const;

    //��Ҫ�ײ�����˸�ۺ���
    void onSignalTrayIconTwinkle(const bool bNeed);
    //��˸��ʱ����ʱ
    void onSignalIconTwinkleTimerout();

private:
    void initUi();
    void initConnect();
    void initData();
    //void notifyServerOnline();
    void getLastChatFromBackup(std::vector<QString>& tmpOrder,QSqlDatabase& db);
    //��һ����ʱ�����ݿ����ͱ������ݿ��������
    void connectToBackupDB(QSqlDatabase& db);
    //�����˾Ͱ���ʱ���ݿ�Ͽ�����
    void disConnectBackupDB(QSqlDatabase& db)const;

private:
    Ui::ChatWidget *ui;
    QObject* m_ptrLastChatQMLRoot{ nullptr }; //�ϴ�����qml�ĸ�����
    QObject* m_ptrFriendListQMLRoot{ nullptr };//�����б�qml�ĸ�����
    QObject* m_ptrAddFriendQMLRoot{ nullptr };  //��Ӻ���qml����ĸ�����
    QTimer* m_ptrNullMessageTimer{ nullptr };   //�յ��������
    QQuickWidget* m_ptrLastChatWidget{ nullptr }; //�ϴ������б����
    QQuickWidget* m_ptrFriendListWidget{ nullptr }; //�����б����
    QQuickWidget* m_ptrNewFriendAndAreadyAddWidget{ nullptr };  //��ʾ����Ӻ��º�������Ľ���
    QQuickWidget* m_ptrSearchFriendList{ nullptr };//��������ʱ��ʾ�Ľ���
    QQuickWidget* m_ptrEmptyWid{ nullptr };     //�յĽ���
    QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //����ͼ��
    QTimer* m_ptrIconTwinkleTimer{ nullptr };   //����ͼ����˸�ö�ʱ��
    TrayIconState m_iTrayState{ TrayIconState::Normal };        //����ͼ���״̬���Ƿ�Ϊ��˸

    //����û���id
    QString m_strUserId{""};
    QString m_strUserName{ "" };
    //�洢������Ϣ��������ĸ������Ǵӷ������õ��ģ��о�����Ϣ
    /*std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::unordered_map<QString, int> m_mapUserInfo;     //�洢����id�͸�id��vec�ж�Ӧ��λ�ã��Ա������Ϣ
    std::vector<MyLastChatFriendInfo> m_vecLastChatFriend;*/        //�ϴ�����ҳ����ĺ���,����Ǵ����ݿ�õ���˳��ֻ��id������������Ϣ
};
