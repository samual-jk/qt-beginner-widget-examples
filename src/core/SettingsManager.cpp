#include "SettingsManager.h"

#include <QSettings>

static const char* ORGANIZATION = "NotepadPro";
static const char* APPLICATION = "NotepadPro";

QStringList SettingsManager::loadRecentFiles()
{
    QSettings settings(ORGANIZATION, APPLICATION);
    return settings.value("recentFiles").toStringList();
}

void SettingsManager::saveRecentFiles(const QStringList& files)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.setValue("recentFiles", files);
}

QFont SettingsManager::loadFontPreferences(const QString& user)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    QString family = settings.value("fontFamily", "Consolas").toString();
    int size = settings.value("fontSize", 12).toInt();
    int weight = settings.value("fontWeight", QFont::Normal).toInt();
    settings.endGroup();

    QFont font;
    font.setFamily(family);
    font.setPointSize(size);
    font.setWeight(static_cast<QFont::Weight>(weight));

    return font;
}

void SettingsManager::saveFontPreferences(const QString& user, const QFont& font)
{
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup("users/" + user + "/preferences");

    settings.setValue("fontFamily", font.family());
    settings.setValue("fontSize", font.pointSize());
    settings.setValue("fontWeight", font.weight());

    settings.endGroup();
}

bool SettingsManager::loadWordWrap(const QString& user)
{
    QSettings settings(ORGANIZATION, APPLICATION);

    settings.beginGroup("users/" + user + "/preferences");
    bool wrap = settings.value("wordWrap", true).toBool();
    settings.endGroup();

    return wrap;
}

void SettingsManager::saveWordWrap(const QString& user, bool enabled)
{
    QSettings settings(ORGANIZATION, APPLICATION);
    settings.beginGroup("users/" + user + "/preferences");
    settings.setValue("wordWrap", enabled);
    settings.endGroup();
}