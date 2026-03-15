#pragma once

#include <QString>
#include <QStringList>

class UserManager
{
public:

    static QStringList users();

    static bool userExists(const QString& username);

    static bool createUser(const QString& username,
        const QString& password);

    static bool authenticate(const QString& username,
        const QString& password);

    static QString hashPassword(const QString& password);

    static void setCurrentUser(const QString& username);

    static QString currentUser();

    static void logout();

    static constexpr int MaxUsers = 5;
};