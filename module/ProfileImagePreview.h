#pragma once

#include <QWidget>

class ProfileImagePreview  : public QWidget
{
    Q_OBJECT

public:
    ProfileImagePreview(QWidget *parent);
    ~ProfileImagePreview();
    ProfileImagePreview(const ProfileImagePreview&) = delete;
    ProfileImagePreview& operator=(const ProfileImagePreview&) = delete;

    void setImagePath(const QString& path,const int id);
    
protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_iPageId{ -1 };
    QString m_strPagePath{ "" };
};
