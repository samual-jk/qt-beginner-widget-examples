#pragma once

#include <QDialog>

namespace Ui {
    class SignupDialog;
}

class SignupDialog : public QDialog
{
    Q_OBJECT

public:

    explicit SignupDialog(QWidget* parent = nullptr);

    ~SignupDialog();

private:

    Ui::SignupDialog* ui;

private slots:

    void createUser();
};