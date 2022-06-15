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
#include <map>
#include <unordered_set>

namespace Ui 
{ 
    class ChatWidget; 
};

enum TrayIconState 
{
    NormalState,
    BlinkState
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
    ChatWidget(QString id,QWidget *parent = Q_NULLPTR);
    ~ChatWidget();
    ChatWidget(const ChatWidget& l) = delete;
    ChatWidget& operator=(const ChatWidget& l) = delete;

protected slots:
    //�¼�������������
    bool eventFilter(QObject* watched, QEvent* event)override;

public:
    //���ϴ������б������һ���µģ�һ����������µĺ��ѵ�ʱ�����
    void onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo);

private slots:
    //�ı��������ý���
    void onSignalTextEditIsFocus(bool isFocus);
    //������Ϣ��ť�������
    void onSignalSendMessage();
    //�ϴ������б������
    void onSignalLastChatItemClicked(QString strId);
    //�����б����
    void onSignalFriendListItemClicked(QString strId);
    //�ײ����̱������
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);
    //�յ�����������Ϣ��
    void onSignalSingleChatMessage(const QString& chatMessage);
    //���������ʧȥ�����
    void onSignalSearchTextLoseFocus(bool isFocus);
    //���������Ự��ť
    void onSignalChatBtn();
    //������ͨѶ¼��ť
    void onSignalFriendListBtn();
    //����������Ӻ��Ѱ�ť
    void onSignalAddFriendBtn();
    //�յ������б���Ϣ
    void onSignalRecvFriendList(const QString& friendList);
    //manager��������б����
    void initFriendList();
    //����id��ʼ����id��Ӧ���������
    void initChatMessageWidAcordId(QString strId);
    //��ʼ���ϴ������б�
    void initLastChatList();
    //��ʼ���������
    void initAllChatWid();

private:
    void initUi();
    void initConnect();
    void initData();
    //void notifyServerOnline();

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
    //����û���id
    QString m_strUserId{ "" };
    int m_iTrayState{ NormalState };        //����ͼ���״̬���Ƿ�Ϊ��˸

        //�洢������Ϣ��������ĸ
    std::vector<MyFriendInfoWithFirstC> m_vecFriendInfoWithC;
    std::map<QString, int> m_mapUserInfo;     //�洢����id�͸�id��vec�ж�Ӧ��λ�ã��Ա������Ϣ
    std::vector<QString> m_vecLastChatFriend;        //�ϴ�����ҳ����ĺ���
};
