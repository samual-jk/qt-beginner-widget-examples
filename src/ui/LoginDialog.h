#pragma once

#include <QDialog>

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:

    explicit LoginDialog(QWidget* parent = nullptr);

    ~LoginDialog();

    QString username() const;

private:

    Ui::LoginDialog* ui;

    QString m_username;

private slots:

    void login();
    void signup();
};