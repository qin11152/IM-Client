#pragma once

#include <QWidget>
#include "ui_ProfileImagePreview.h"

class ProfileImagePreview : public QWidget
{
    Q_OBJECT

public:
    ProfileImagePreview(QWidget *parent = nullptr);
    ~ProfileImagePreview();
    ProfileImagePreview(const ProfileImagePreview&) = delete;
    ProfileImagePreview& operator=(const ProfileImagePreview&) = delete;

    void setImagePath(const QString& path, const int id);

signals:
    void signalProfileImageChanged(QImage& image);
    //����ͼƬ��Ϣ�ź�
    void signalSendImageMsg(const QString& strBase64Image, const QString& imageName, const QString& suffix);

private slots :
    void onSignalChooseBtnClicked();
    void onSignalUpdateProfileImage();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    void initConnect();
    //��ѡ���ͼƬѹ���������������
    void compressAndSendImage(const QImage& image);
    //ɾ�����ݿ���·����ͼƬ������ͼƬ����·�����½�ȥ
    void saveImageAndUpdateDB(const QImage& image);

private:
    int m_iPageId{ -1 };
    QString m_strPagePath{ "" };
    Ui::ProfileImagePreviewClass ui;
};
