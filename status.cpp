// ===================== EditorWidget.h (UPDATE) =====================
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
    void cursorPositionChanged(int line, int column);

private:
    QPlainTextEdit* m_editor = nullptr;
    std::unique_ptr<Document> m_document;
};


// ===================== EditorWidget.cpp (UPDATE) =====================
#include "EditorWidget.h"
#include "../core/Document.h"

#include <QPlainTextEdit>
#include <QVBoxLayout>
#include <QTextDocument>
#include <QTextCursor>

EditorWidget::EditorWidget(QWidget* parent)
    : QWidget(parent),
      m_document(std::make_unique<Document>())
{
    m_editor = new QPlainTextEdit(this);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0,0,0,0);
    layout->addWidget(m_editor);

    connect(m_editor->document(), &QTextDocument::modificationChanged,
        this, [this](bool modified)
        {
            m_document->setModified(modified);
            emit modificationChanged(modified);
        });

    connect(m_editor, &QPlainTextEdit::cursorPositionChanged,
        this, [this]()
        {
            QTextCursor cursor = m_editor->textCursor();

            int line = cursor.blockNumber() + 1;
            int column = cursor.positionInBlock() + 1;

            emit cursorPositionChanged(line, column);
        });
}

Document* EditorWidget::document() const
{
    return m_document.get();
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


// ===================== MainWindow.h (UPDATE) =====================

#include <QLabel>

private:
    QLabel* m_statusPosition = nullptr;
    QLabel* m_statusEncoding = nullptr;
    QLabel* m_statusWrap = nullptr;
    QLabel* m_statusUser = nullptr;


// ===================== MainWindow.cpp (STATUS BAR SETUP) =====================
#include "../core/UserManager.h"

void MainWindow::setupUi()
{
    ui->setupUi(this);

    m_statusPosition = new QLabel("Ln 1, Col 1", this);
    m_statusEncoding = new QLabel("UTF-8", this);
    m_statusWrap = new QLabel("Wrap: ON", this);
    m_statusUser = new QLabel(this);

    statusBar()->addPermanentWidget(m_statusPosition);
    statusBar()->addPermanentWidget(m_statusEncoding);
    statusBar()->addPermanentWidget(m_statusWrap);
    statusBar()->addPermanentWidget(m_statusUser);

    m_statusUser->setText("User: " + UserManager::currentUser());
}


// ===================== CONNECT EDITOR TO STATUS =====================

void MainWindow::addEditorTab(EditorWidget* editor, const QString& title)
{
    ui->tabWidget->addTab(editor, title);
    ui->tabWidget->setCurrentWidget(editor);

    connect(editor, &EditorWidget::cursorPositionChanged,
        this, [this](int line, int col)
        {
            m_statusPosition->setText(
                QString("Ln %1, Col %2").arg(line).arg(col));
        });

    connect(editor, &EditorWidget::modificationChanged,
        this, [this, editor](bool modified)
        {
            int index = ui->tabWidget->indexOf(editor);
            if(index == -1) return;

            QString title = editor->document()->displayName();
            if(modified) title += "*";

            ui->tabWidget->setTabText(index, title);
        });
}


// ===================== WORD WRAP STATUS UPDATE =====================

connect(ui->actionWordWrap,
        &QAction::toggled,
        this,
        [this](bool enabled)
        {
            auto* editor = currentEditor();
            if(!editor) return;

            editor->setWordWrap(enabled);

            m_statusWrap->setText(
                enabled ? "Wrap: ON" : "Wrap: OFF");
        });


// ===================== TAB SWITCH SYNC =====================

connect(ui->tabWidget,
        &QTabWidget::currentChanged,
        this,
        [this](int)
        {
            auto* editor = currentEditor();
            if(!editor) return;

            QTextCursor cursor = editor->editor()->textCursor();

            int line = cursor.blockNumber() + 1;
            int col = cursor.positionInBlock() + 1;

            m_statusPosition->setText(
                QString("Ln %1, Col %2").arg(line).arg(col));
        });


// ===================== USER SWITCH UPDATE =====================

void MainWindow::updateUserStatus()
{
    m_statusUser->setText(
        "User: " + UserManager::currentUser());
}
