#include "FileManager.h"

#include <QFile>
#include <QTextStream>

std::optional<QString> FileManager::readFile(const QString& path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return std::nullopt;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    return in.readAll();
}

bool FileManager::writeFile(const QString& path, const QString& content)
{
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << content;

    return true;
}