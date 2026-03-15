#pragma once
#include <QString>
#include <optional>

class FileManager
{
public:
    static std::optional<QString> readFile(const QString& path);
    static bool writeFile(const QString& path, const QString& content);
};