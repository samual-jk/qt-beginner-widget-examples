#include "Document.h"
#include <QFileInfo>

void Document::setFilePath(const QString& path)
{
    m_filePath = path;
    m_isUntitled = false;
}

QString Document::filePath() const
{
    return m_filePath;
}

QString Document::displayName() const
{
    if (m_isUntitled)
        return "Untitled";

    return QFileInfo(m_filePath).fileName();
}

bool Document::isModified() const
{
    return m_modified;
}

void Document::setModified(bool modified)
{
    m_modified = modified;
}

bool Document::isUntitled() const
{
    return m_isUntitled;
}