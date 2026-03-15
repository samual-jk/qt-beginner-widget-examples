#include "UserManager.h"

#include <QSettings>
#include <QCryptographicHash>

static const char* ORGANIZATION = "NotepadPro";
static const char* APPLICATION = "NotepadPro";

static const char* SESSION_GROUP = "session";
static const char* CURRENT_USER = "currentUser";

QStringList UserManager::users()
{
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup("users");

    QStringList list = settings.childGroups();

    settings.endGroup();

    return list;
}

bool UserManager::userExists(const QString& username)
{
    return users().contains(username);
}

bool UserManager::createUser(const QString& username,
    const QString& password)
{
    if (userExists(username))
        return false;

    if (users().size() >= MaxUsers)
        return false;

    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup("users/" + username);

    settings.setValue("password", hashPassword(password));

    settings.endGroup();

    return true;
}

bool UserManager::authenticate(const QString& username,
    const QString& password)
{
    if (!userExists(username))
        return false;

    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup("users/" + username);

    QString stored = settings.value("password").toString();

    settings.endGroup();

    return stored == hashPassword(password);
}

QString UserManager::hashPassword(const QString& password)
{
    QByteArray hash =
        QCryptographicHash::hash(
            password.toUtf8(),
            QCryptographicHash::Sha256);

    return hash.toHex();
}

void UserManager::setCurrentUser(const QString& username)
{
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup(SESSION_GROUP);
    settings.setValue(CURRENT_USER, username);
    settings.endGroup();
}

QString UserManager::currentUser()
{
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup(SESSION_GROUP);

    QString user = settings.value(CURRENT_USER).toString();

    settings.endGroup();

    return user;
}

void UserManager::logout()
{
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup(SESSION_GROUP);
    settings.remove(CURRENT_USER);
    settings.endGroup();
}

