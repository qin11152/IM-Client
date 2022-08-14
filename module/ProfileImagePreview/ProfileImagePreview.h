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

    private slots :
    void onSignalChooseBtnClicked();
    void onSignalUpdateProfileImage();

protected:
    void paintEvent(QPaintEvent* event) override;
    void showEvent(QShowEvent* event) override;

private:
    int m_iPageId{ -1 };
    QString m_strPagePath{ "" };
    Ui::ProfileImagePreviewClass ui;
};
