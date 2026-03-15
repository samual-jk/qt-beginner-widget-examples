#include "SettingsManager.h"

#include <QSettings>s

void SettingsManager::saveWindowState(QMainWindow*) {}
void SettingsManager::restoreWindowState(QMainWindow*) {}

void SettingsManager::saveRecentFiles(const QStringList& files) 
{
	QSettings settings("NotepadPro", "NotepadPro");
	settings.setValue("recentFiles", files);
}

QStringList SettingsManager::loadRecentFiles() 
{
	QSettings settings("NotepadPro", "NotepadPro");
	return settings.value("recentFiles").toStringList();
}