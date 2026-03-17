// ===================== SettingsManager.h =====================
#pragma once

#include <QString>
#include <QStringList>
#include <QFont>

class SettingsManager
{
public:
    static QStringList loadRecentFiles(const QString& user);
    static void saveRecentFiles(const QString& user, const QStringList& files);

    static QString loadLastOpenedFile(const QString& user);
    static void saveLastOpenedFile(const QString& user, const QString& path);

    static bool loadOpenLastFile(const QString& user);
    static void saveOpenLastFile(const QString& user, bool enabled);
};


// ===================== SettingsManager.cpp =====================
#include "SettingsManager.h"
#include <QSettings>

static const char* ORGANIZATION = "NotepadPro";
static const char* APPLICATION = "NotepadPro";

QStringList SettingsManager::loadRecentFiles(const QString& user)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    QStringList files = settings.value("recentFiles").toStringList();
    settings.endGroup();
    return files;
}

void SettingsManager::saveRecentFiles(const QString& user, const QStringList& files)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    settings.setValue("recentFiles", files);
    settings.endGroup();
}

QString SettingsManager::loadLastOpenedFile(const QString& user)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    QString path = settings.value("lastFile").toString();
    settings.endGroup();
    return path;
}

void SettingsManager::saveLastOpenedFile(const QString& user, const QString& path)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    settings.setValue("lastFile", path);
    settings.endGroup();
}

bool SettingsManager::loadOpenLastFile(const QString& user)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    bool value = settings.value("openLastFile", false).toBool();
    settings.endGroup();
    return value;
}

void SettingsManager::saveOpenLastFile(const QString& user, bool enabled)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    settings.setValue("openLastFile", enabled);
    settings.endGroup();
}


// ===================== MainWindow.cpp (CHANGES ONLY) =====================

#include "../core/SettingsManager.h"
#include "../core/UserManager.h"

// ---------- Constructor addition ----------
QString user = UserManager::currentUser();

m_recentFiles = SettingsManager::loadRecentFiles(user);
updateRecentFileActions();

// Open last file on startup
if (SettingsManager::loadOpenLastFile(user))
{
    QString lastFile = SettingsManager::loadLastOpenedFile(user);

    if (!lastFile.isEmpty() && QFileInfo::exists(lastFile))
    {
        auto content = FileManager::readFile(lastFile);

        if (content)
        {
            auto* editor = new EditorWidget(this);
            editor->setText(*content);
            editor->document()->setFilePath(lastFile);

            addEditorTab(editor, QFileInfo(lastFile).fileName());
        }
    }
}


// ---------- addRecentFile ----------
void MainWindow::addRecentFile(const QString& path)
{
    QString user = UserManager::currentUser();

    m_recentFiles.removeAll(path);
    m_recentFiles.prepend(path);

    while (m_recentFiles.size() > MaxRecentFiles)
        m_recentFiles.removeLast();

    SettingsManager::saveRecentFiles(user, m_recentFiles);

    SettingsManager::saveLastOpenedFile(user, path);

    updateRecentFileActions();
}


// ---------- openFile (add lines at end) ----------
QString user = UserManager::currentUser();
SettingsManager::saveLastOpenedFile(user, path);
addRecentFile(path);


// ---------- saveFileAs (add lines at end) ----------
QString user = UserManager::currentUser();
SettingsManager::saveLastOpenedFile(user, path);
addRecentFile(path);
