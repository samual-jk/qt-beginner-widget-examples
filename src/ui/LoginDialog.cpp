#include "LoginDialog.h"
#include "ui_LoginDialog.h"

#include "SignupDialog.h"
#include "../core/UserManager.h"

#include <QMessageBox>

LoginDialog::LoginDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);

    connect(ui->loginBtn,
        &QPushButton::clicked,
        this,
        &LoginDialog::login);

    connect(ui->signupBtn,
        &QPushButton::clicked,
        this,
        &LoginDialog::signup);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

QString LoginDialog::username() const
{
    return m_username;
}

void LoginDialog::login()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();

    if (!UserManager::authenticate(username, password))
    {
        QMessageBox::warning(this, "Login Failed", "Invalid credentials");
        return;
    }

    UserManager::setCurrentUser(username);

    m_username = username;

    accept();
}

void LoginDialog::signup()
{
    SignupDialog dialog(this);

    dialog.exec();
}