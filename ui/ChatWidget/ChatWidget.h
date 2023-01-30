/************************************************************************/
/*���������棬���û���¼���������Ȳ����Ľ��棬Ҳ����Ϊ��Ҫ�Ľ��� 
*/
/************************************************************************/

#pragma once

#include "module/stdafx.h"
#include "ui/AddFriendWidget/AddFriendWidget.h"
#include "ui/ProfileImagePreview/ProfileImagePreview.h"
#include "ui/MyChatMessageQuickWid/MyChatMessageQuickWid.h"
#include "ui/ChatWidget/FriendListModel/MyFriendListModel.h"
#include "ui/ChatWidget/FriendListModel/MyFriendListSortModel.h"

#include <QTimer>
#include <QWidget>
#include <QQuickWidget>
#include <QSqlDatabase>
#include <QSystemTrayIcon>


namespace Ui 
{ 
	class ChatWidget; 
};


/**
 * brief: ��������еļ���widget�������յĽ��棬��Ӻ��ѵĽ�����������.
 */
enum ChatStackedWidgetCommonWidget
{
	AddFriendWid = 19999,
	EmptyWid,
	ChatWid,
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

	//���ϴ������б������һ���µģ�һ����������µĺ��ѵ�ʱ�����
	void onSignalAdd2LastChat(const MyFriendInfoWithFirstC& friendInfo);

protected slots:
	//�¼�������������
	bool eventFilter(QObject* watched, QEvent* event)override;

private slots:
	//manager��������б����
	void initFriendList();
	/**
	 * brief.��ʼ����id��Ӧ��������棬�����ʮ�������ʮ�������¼��û�о��м�����Ӽ���
	 * 
	 * \param lastChatInfo��������id
	 */
	void initChatMessageWidAcordId(const MyLastChatFriendInfo& lastChatInfo);
	//��ʼ���ϴ������б�
	void initLastChatList();
	//��ʼ���������
	void initAllChatWid();
	//��ʼ����Ӻ��ѽ���
	void initAddFriendWid();

	//���������ʧȥ�����
	void onSignalSearchTextLoseFocus(bool isFocus);

	/**
	 * brief��������Ϣ���е����ݷ����˱仯.
	 * 
	 * \param searchMsg������������
	 */
	void onSignalSearchTextChange(const QString& searchMsg);

	//������Ϣ��ť�������
	void onSignalSendMessage();
	//�ϴ������б������
	void onSignalLastChatItemClicked(const QString strId, const QString strName);
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
	//��������ͷ��ť
	void onSignalSideBarProfileImageBtn();

	//��ĳ�����ѵ�����ҳ��Ҫ��ˢ�½��棬Ҳ����Ҫ�����������¼
	void onSignalUpdateChatMessage(const QString id);
	//���ú�ɫ��ʾ��ʾ��������ȷ������
	void onSignalSetRedRectangleShow(const QString& id)const;
	//�����ȡ����ɫ��Ϣ��ʾ��
	void onSignalHideRedRectangleInLastChat(const QString id);

	//��������ĸ��Ӣ�ĶԺ����б�����
	void onSortFriendList();
	//ˢ�½����еĺ����б�
	void onUpdateFriendListUI()const;
	//QML����֪ͨ�ϴ����������Ҫд�����ݿ�
	void onSignalNeedUpdateLastChat()const;
	//QMLҳ��֪ͨ��ͷ��Ԥ��
	void onSignalChatWidProfileImageClicked(const QString id);

	//��Ҫ�ײ�����˸�ۺ���
	void onSignalTrayIconTwinkle(const bool bNeed);
	//��˸��ʱ����ʱ
	void onSignalIconTwinkleTimerout();

	/**
	 * brief���޸Ĳ������ͷ��.
	 * 
	 * \param strImage�������ͷ��·��
	 */
	void setProfileImage(const QString& strImage);
	/**
	 * brief:Ҫ���ͷ��Ԥ�������Ľ���.
	 * 
	 * \param id�������id������id��ʾ��Ӧ��ͷ�����id���Լ��������޸İ�ť
	 */
	void onSignalChatWidOpenProfileImagePreview(const int id);
	/**
	 * brief���޸����Լ�ͷ��·����Ҫ���������������Ͻ�ͷ�����.
	 * 
	 * \param imagePath���޸ĺ��ͷ��·��
	 */
	void onSignalProfileImageChanged(const QString& id, const QString& imagePath);

	/**
	 * brief:���ѵ�ͷ���޸����Ժ�͸���һ��lastchat��Friendlist�����鿴�ǲ��Ǻ��������أ��ǾͰ�chatwidgetҲ����һ��.
	 * 
	 * \param id������id
	 * \param imagePath��������ͷ��·��
	 */
	void onSignalFriendProfileImageChanged(const QString& id, const QString& imagePath)const;
	
	/**
	 * brief��������������һ���������ѵ��˵�ͷ�񣬸��µ�model�����ݿ���.
	 * 
	 * \param id��id
	 * \param imagePath��ͷ��·��
	 */
	void onSignalAddFriendProfileImage(const QString& id, const QString& imagePath)const;

private:
	/**
	 * brief����ʼ��������ص�����.
	 * 
	 */
	void initUi();
	/**
	 * brief:��ʼ���źŲ�����.
	 * 
	 */
	void initConnect();
	//************************************
	// Method:    initData
	// FullName:  ChatWidget::initData
	// Access:    private 
	// Returns:   void
	// Qualifier: �����������Ҫ���г�ʼ������ȡһЩ����
	//************************************
	void initData();
	/**
	 * brief:�ӱ������ݿ��л�ȡ�ϴ������б���Ϊ�޸�˳��������̱߳����ڱ������ݿ���.
	 * 
	 * \param tmpOrder:��ȡ�����ϴ���������
	 * \param db Ҫ���ӵ����ݿ�
	 */
	void getLastChatFromBackup(std::vector<QString>& tmpOrder,QSqlDatabase& db);
	/**
	 * brief���ͱ������ݿ�������ӣ�����֮������̲߳���.
	 * 
	 * \param db �������ݿ�
	 */
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
	//QQuickWidget* m_ptrNewFriendAndAreadyAddWidget{ nullptr };  //��ʾ����Ӻ��º�������Ľ���
	QQuickWidget* m_ptrSearchFriendList{ nullptr };//��������ʱ��ʾ�Ľ���
	QQuickWidget* m_ptrEmptyWid{ nullptr };     //�յĽ���
	QSystemTrayIcon* m_ptrTrayIcon{ nullptr };  //����ͼ��
	QTimer* m_ptrIconTwinkleTimer{ nullptr };   //����ͼ����˸�ö�ʱ��
	TrayIconState m_iTrayState{ TrayIconState::Normal };        //����ͼ���״̬���Ƿ�Ϊ��˸
	ProfileImagePreview* m_ptrProfileImagePreviewWid{ nullptr };     //ͷ��Ԥ������
	MyChatMessageQuickWid* m_ptrChatMessageWid{ nullptr }; //ʵ�ʵ��������
	AddFriendWidget* m_ptrAddFriendWid{ nullptr }; //��Ӻ��ѽ���
	MyFriendListSortModel* m_ptrFriendListModel{ nullptr }; //�����б�model

	//����û���id
	QString m_strUserId{""};
	QString m_strUserName{ "" };
	bool m_bLastChatInitFinished{ false };
	std::mutex m_mutex;
	std::condition_variable m_con;
};
