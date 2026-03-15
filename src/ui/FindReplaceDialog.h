#pragma once

#include "ui_FindReplaceDialog.h"

#include <QDialog>
#include <QTextDocument>

class QPlainTextEdit;
class QTextDocument;

namespace Ui {
    class FindReplaceDialog;
}

class FindReplaceDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FindReplaceDialog(QWidget* parent = nullptr);
    ~FindReplaceDialog() override;

    void setEditor(QPlainTextEdit* editor);
    void highlightAll(const QString& text);
    void clearHighlights();

private slots:
    void onFindNext();
    void onFindPrevious();
    void onReplace();
    void onReplaceAll();
    void onFindAll();

private:
    Ui::FindReplaceDialog* ui;
    QPlainTextEdit* m_editor = nullptr;

    QTextDocument::FindFlags findFlags() const;
};