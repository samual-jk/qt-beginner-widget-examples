#include "ui/MainWindow.h"
#include "src/ui/MainWindow.h"
#include "src/ui/LoginDialog.h"
#include "src/ui/SignupDialog.h"
#include "src/core/UserManager.h"

#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString currentUser = UserManager::currentUser();

    if (currentUser.isEmpty())
    {
        if (UserManager::users().isEmpty())
        {
            SignupDialog signup;

            if (signup.exec() != QDialog::Accepted)
                return 0;
        }

        LoginDialog login;

        if (login.exec() != QDialog::Accepted)
            return 0;
    }

    MainWindow window;
    window.show();
    return app.exec();
}
