#pragma once

#include <QString>
#include <QStringList>
#include <QFont>

class SettingsManager
{
public:

    static QStringList loadRecentFiles();
    static void saveRecentFiles(const QStringList& files);

    static QFont loadFontPreferences(const QString& user);
    static void saveFontPreferences(const QString& user, const QFont& font);

    static bool loadWordWrap(const QString& user);
    static void saveWordWrap(const QString& user, bool enabled);
};