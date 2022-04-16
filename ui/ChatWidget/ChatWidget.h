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

namespace Ui 
{ 
    class ChatWidget; 
};

enum TrayIconState {
    NormalState,
    BlinkState
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
    void onSignalSwitchFriendListWidget(int type);
    //�����б������
    void onSignalFriendListClicked(int iId);
    //�ײ����̱������
    void onSignalTrayTriggered(QSystemTrayIcon::ActivationReason reason);
    //�յ�����������Ϣ��
    void onSignalSingleChatMessage(QString& chatMessage);
    void onStateChanged(QQuickWidget::Status status);
    void sceneGraphError(QQuickWindow::SceneGraphError, const QString& message);

private:
    void initUi();
    void initConnect();
    void initData();
    void getLastChatListFromDB();
    void notifyServerOnline();
    //void getFriendListFromServer();

private:
    Ui::ChatWidget *ui;
    QObject* m_ptrFriendListQMLRoot{ nullptr }; //qml�ĸ�����
    QTimer* m_ptrNullMessageTimer{ nullptr };   //�յ��������
    QQuickWidget* m_ptrFriendListWidget{ nullptr }; //�ϴ������б����
    QQuickWidget* m_ptrSearchWidget{ nullptr }; //�������ѽ���
    QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //����ͼ��
    //����û���id
    int m_iId{ -1 };

    int m_iTrayState{ NormalState };        //����ͼ���״̬���Ƿ�Ϊ��˸
};
