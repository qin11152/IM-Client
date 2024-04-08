#pragma once

#include "ui_ProfileImagePreview.h"

#include <QWidget>

namespace wechat
{
    class ProfileImagePreview : public QWidget
    {
        Q_OBJECT

    public:
        ProfileImagePreview(QWidget* parent = nullptr);
        ~ProfileImagePreview();
        ProfileImagePreview(const ProfileImagePreview&) = delete;
        ProfileImagePreview& operator=(const ProfileImagePreview&) = delete;

        void setImagePath(const QString& path, const int id);

    signals:
        void signalProfileImageChanged(const QString& id, const QString& path);

    private slots:
        void onSignalChooseBtnClicked();
        void onSignalUpdateProfileImage();

    protected:
        void paintEvent(QPaintEvent* event) override;
        void showEvent(QShowEvent* event) override;

    private:
        void initConnect();
        //将选择的图片压缩并传输给服务器
        void compressAndSendImage(const QImage& image, const std::string& timeStamp);
        //删除数据库中路径下图片，保存图片并将路径更新进去
        void saveImageAndUpdateDB(const QImage& image, const std::string& timeStamp);

    private:
        int m_iPageId{ -1 };
        QString m_strPagePath{ "" };
        Ui::ProfileImagePreviewClass ui;
    };
}
