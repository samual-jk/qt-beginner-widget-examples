#pragma once
#include <QStringList>

class QMainWindow;

class SettingsManager
{
public:
    static void saveWindowState(QMainWindow* window);
    static void restoreWindowState(QMainWindow* window);

    static void saveRecentFiles(const QStringList& files);
    static QStringList loadRecentFiles();
};