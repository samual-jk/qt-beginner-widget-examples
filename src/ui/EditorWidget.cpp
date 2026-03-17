#include "EditorWidget.h"
#include "../core/Document.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTextDocument>

EditorWidget::EditorWidget(QWidget* parent)
    : QWidget(parent),
    m_document(new Document())
{
    m_editor = new QPlainTextEdit(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_editor);

    connect(m_editor->document(), &QTextDocument::modificationChanged,
        this, [this](bool modified)
        {
            m_document->setModified(modified);
            emit modificationChanged(modified);
        });
}

Document* EditorWidget::document() const
{
    return m_document;
}

QString EditorWidget::text() const
{
    return m_editor->toPlainText();
}

void EditorWidget::setText(const QString& text)
{
    m_editor->setPlainText(text);
    m_document->setModified(false);
}

QPlainTextEdit* EditorWidget::editor() const
{
    return m_editor;
}

void EditorWidget::setEditorFont(const QFont& font)
{
    m_editor->setFont(font);
}

void EditorWidget::setWordWrap(bool enabled)
{
    m_editor->setLineWrapMode(
        enabled
        ? QPlainTextEdit::WidgetWidth
        : QPlainTextEdit::NoWrap
    );
}