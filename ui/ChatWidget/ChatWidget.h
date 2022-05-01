/************************************************************************/
/*���������棬���û���¼���������Ȳ����Ľ��棬Ҳ����Ϊ��Ҫ�Ľ��� 
*/
/************************************************************************/

#pragma once

#include "../../module/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include <QWidget>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QQuickWidget>
#include <map>

namespace Ui 
{ 
    class ChatWidget; 
};

enum TrayIconState {
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
    ChatWidget(int id,QWidget *parent = Q_NULLPTR);
    ~ChatWidget();
    ChatWidget(const ChatWidget& l) = delete;
    ChatWidget& operator=(const ChatWidget& l) = delete;

protected slots:
    //�¼�������������
    bool eventFilter(QObject* watched, QEvent* event)override;

private slots:
    //�ı��������ý���
    void onSignalTextEditIsFocus(bool isFocus);
    //������Ϣ��ť�������
    void onSignalSendMessage();
    void onSignalSwitchLastChatWidget(int type);
    //�����б������
    void onSignalFriendListClicked(int iId);
    //�ײ����̱������
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);
    //�յ�����������Ϣ��
    void onSignalSingleChatMessage(const QString& chatMessage);
    //�յ�qmlҳ��ͬ����Ӻ��ѵ�����
    void onSignalAgreeAddFriend(QString friendName);
    //�յ������б��
    void onSignalFriendList(const QString& friendList);

private:
    void initUi();
    void initConnect();
    void initData();
    void getLastChatListFromDB();
    void notifyServerOnline();
    //��ȡ�����б�
    void getFriendList();
    //void getFriendListFromServer();

private:
    Ui::ChatWidget *ui;
    QObject* m_ptrLastChatQMLRoot{ nullptr }; //�ϴ�����qml�ĸ�����
    QObject* m_ptrFriendListQMLRoot{ nullptr };//�����б�qml�ĸ�����
    QTimer* m_ptrNullMessageTimer{ nullptr };   //�յ��������
    QQuickWidget* m_ptrLastChatWidget{ nullptr }; //�ϴ������б����
    QQuickWidget* m_ptrFriendListWidget{ nullptr }; //��������������
    QQuickWidget* m_ptrNewFriendAndAreadyAddWidget{ nullptr };  //��ʾ����Ӻ��º�������Ľ���
    QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //����ͼ��
    //����û���id
    int m_iId{ -1 };
    std::map<std::string, std::string> m_mapUserInfo;     //�洢�û�id�Ͷ�Ӧ���û���
    int m_iTrayState{ NormalState };        //����ͼ���״̬���Ƿ�Ϊ��˸
};
