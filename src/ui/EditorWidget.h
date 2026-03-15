#pragma once

#include <QWidget>
#include <memory>

class QPlainTextEdit;
class Document;

class EditorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EditorWidget(QWidget* parent = nullptr);
    ~EditorWidget() override = default;

    Document* document() const;
    QString text() const;
    void setText(const QString& text);
    QPlainTextEdit* editor() const;

signals:
    void modificationChanged(bool modified);

private:
    QPlainTextEdit* m_editor = nullptr;
    Document* m_document;
};