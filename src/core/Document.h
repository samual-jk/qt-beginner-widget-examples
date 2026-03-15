#pragma once

#include <QString>

class Document
{
public:
    Document() = default;

    void setFilePath(const QString& path);
    QString filePath() const;

    QString displayName() const;

    bool isModified() const;
    void setModified(bool modified);

    bool isUntitled() const;

private:
    QString m_filePath;
    bool m_modified = false;
    bool m_isUntitled = true;
};