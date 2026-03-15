#include "SignupDialog.h"
#include "ui_SignupDialog.h"

#include "../core/UserManager.h"

#include <QMessageBox>

SignupDialog::SignupDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::SignupDialog)
{
    ui->setupUi(this);

    connect(ui->createBtn,
        &QPushButton::clicked,
        this,
        &SignupDialog::createUser);

    connect(ui->cancelBtn,
        &QPushButton::clicked,
        this,
        &SignupDialog::reject);
}

SignupDialog::~SignupDialog()
{
    delete ui;
}

void SignupDialog::createUser()
{
    QString username = ui->usernameEdit->text();
    QString password = ui->passwordEdit->text();
    QString confirm = ui->confirmPasswordEdit->text();

    if (username.isEmpty() || password.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Fields cannot be empty");
        return;
    }

    if (password != confirm)
    {
        QMessageBox::warning(this, "Error", "Passwords do not match");
        return;
    }

    if (!UserManager::createUser(username, password))
    {
        QMessageBox::warning(this, "Error", "User exists or limit reached");
        return;
    }

    accept();
}