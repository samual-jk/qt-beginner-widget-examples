#include "FindReplaceDialog.h"

#include <QPlainTextEdit>
#include <QTextDocument>
#include <QTextCursor>
#include <QMessageBox>

FindReplaceDialog::FindReplaceDialog(QWidget* parent)
    : QDialog(parent),
    ui(new Ui::FindReplaceDialog)
{
    ui->setupUi(this);

    connect(ui->findNextBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindNext);
    connect(ui->findPrevBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindPrevious);
    connect(ui->replaceBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplace);
    connect(ui->replaceAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::onReplaceAll);
    connect(ui->findAllBtn, &QPushButton::clicked, this, &FindReplaceDialog::onFindAll);
}

FindReplaceDialog::~FindReplaceDialog()
{
    delete ui;
}

void FindReplaceDialog::setEditor(QPlainTextEdit* editor)
{
    m_editor = editor;
}

QTextDocument::FindFlags FindReplaceDialog::findFlags() const
{
    QTextDocument::FindFlags flags;

    if (ui->caseCheck->isChecked())
        flags |= QTextDocument::FindCaseSensitively;

    if (ui->wholeWordCheck->isChecked())
        flags |= QTextDocument::FindWholeWords;

    return flags;
}

void FindReplaceDialog::onFindNext()
{
    if (!m_editor) return;

    if (!m_editor->find(ui->findEdit->text(), findFlags()))
        QMessageBox::information(this, "Find", "No more matches.");
}

void FindReplaceDialog::onFindPrevious()
{
    if (!m_editor) return;

    auto flags = findFlags() | QTextDocument::FindBackward;

    if (!m_editor->find(ui->findEdit->text(), flags))
        QMessageBox::information(this, "Find", "No more matches.");
}

void FindReplaceDialog::onReplace()
{
    if (!m_editor) return;

    auto cursor = m_editor->textCursor();

    if (cursor.hasSelection())
        cursor.insertText(ui->replaceEdit->text());

    onFindNext();
}

void FindReplaceDialog::onReplaceAll()
{
    if (!m_editor) return;

    QString findText = ui->findEdit->text();
    QString replaceText = ui->replaceEdit->text();

    if (findText.isEmpty())
        return;

    int count = 0;

    m_editor->moveCursor(QTextCursor::Start);

    while (m_editor->find(findText, findFlags()))
    {
        QTextCursor c = m_editor->textCursor();
        c.insertText(replaceText);
        count++;
    }

    QMessageBox::information(this, "Replace All",
        QString("Replaced %1 occurrences").arg(count));
}

void FindReplaceDialog::onFindAll()
{
    if (!m_editor)
        return;

    QString text = ui->findEdit->text();

    if (text.isEmpty())
        return;
    highlightAll(text);
}

void FindReplaceDialog::highlightAll(const QString& text)
{
    clearHighlights();

    QList<QTextEdit::ExtraSelection> selections;

    QTextCursor cursor(m_editor->document());
    int count = 0;

    while (!cursor.isNull() && !cursor.atEnd())
    {
        cursor = m_editor->document()->find(text, cursor, findFlags());

        if (!cursor.isNull())
        {
            QTextEdit::ExtraSelection extra;

            extra.cursor = cursor;
            extra.format.setBackground(Qt::blue);
            if (!cursor.isNull())
                count++;

            selections.append(extra);
        }
    }

    m_editor->setExtraSelections(selections);
    QMessageBox::information(this, "Find All",
        QString("Found %1 matches").arg(count));

}

void FindReplaceDialog::clearHighlights()
{
    if (!m_editor)
        return;

    m_editor->setExtraSelections({});
}