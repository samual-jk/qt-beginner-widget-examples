#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../core/FileManager.h"
#include "../core/Document.h"
#include "../core/SettingsManager.h"
#include "../core/UserManager.h"
#include "EditorWidget.h"
#include "FindReplaceDialog.h"
#include "CustomToolbar.h"
#include "LoginDialog.h"
#include "SignupDialog.h"

#include <QPlainTextEdit>
#include <QTextDocument>
#include <QFileDialog>
#include <QMessageBox>
#include <QFileInfo>
#include <QCloseEvent>
#include <QStringList>
#include <QVector>
#include <QLabel>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupMenus();
    setupUi();
    m_recentFiles = SettingsManager::loadRecentFiles();

    updateRecentFileActions();

    m_customToolbar = new CustomToolbar(this);
    ui->toolBar->addWidget(m_customToolbar);

    QString user = UserManager::currentUser();

    QFont font = SettingsManager::loadFontPreferences(user);
    bool wrap = SettingsManager::loadWordWrap(user);

    m_customToolbar->setFont(font);
    ui->actionWordWrap->setChecked(wrap);

    m_userLabel = new QLabel(this);
    statusBar()->addPermanentWidget(m_userLabel);

    m_userLabel->setText(
        "User: " + user);

    connect(m_customToolbar,
        &CustomToolbar::fontChanged,
        this,
        [this](const QFont& font)
        {
            auto* editor = currentEditor();
            if (!editor) return;

            editor->setEditorFont(font);
        });

    connect(ui->tabWidget,
        &QTabWidget::tabCloseRequested,
        this,
        [this](int index)
        {
            auto* editor =
                qobject_cast<EditorWidget*>(ui->tabWidget->widget(index));

            if (!editor)
                return;

            if (!maybeSave(editor))
                return;

            ui->tabWidget->removeTab(index);
            editor->deleteLater();
        });

    connect(ui->tabWidget,
        &QTabWidget::currentChanged,
        this,
        [this](int)
        {
            auto* editor = currentEditor();
            if (!editor) return;

            m_customToolbar->setFont(
                editor->editor()->font());
        });

    connect(m_customToolbar,
        &CustomToolbar::fontChanged,
        this,
        [this](const QFont& font)
        {
            auto* editor = currentEditor();
            if (!editor) return;

            editor->setEditorFont(font);

            QString user = UserManager::currentUser();

            SettingsManager::saveFontPreferences(user, font);
        });

    connect(ui->actionWordWrap,
        &QAction::toggled,
        this,
        [this](bool enabled)
        {
            auto* editor = currentEditor();
            if (!editor) return;

            editor->setWordWrap(enabled);

            QString user = UserManager::currentUser();

            SettingsManager::saveWordWrap(user, enabled);
        });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupMenus()
{
    connect(ui->actionNew, &QAction::triggered,
        this, &MainWindow::createNewTab);

    connect(ui->actionOpen, &QAction::triggered,
        this, &MainWindow::openFile);

    connect(ui->actionSave, &QAction::triggered,
        this, &MainWindow::saveFile);

    connect(ui->actionSaveAs, &QAction::triggered,
        this, &MainWindow::saveFileAs);

    connect(ui->actionExit, &QAction::triggered,
        this, &MainWindow::close);

    connect(ui->actionLogout,
        &QAction::triggered,
        this,
        &MainWindow::logoutUser);

    connect(ui->actionAddAccount,
        &QAction::triggered,
        this,
        &MainWindow::addAccount);

    connect(ui->actionFindReplace,
        &QAction::triggered,
        this,
        &MainWindow::showFindReplaceDialog);

    m_recentFilesMenu = ui->menuFile->addMenu("Recent Files");

    for (int i = 0; i < MaxRecentFiles; ++i)
    {
        QAction* action = new QAction(this);
        action->setVisible(false);

        connect(action, &QAction::triggered,
            this, &MainWindow::openRecentFile);

        m_recentFileActions.push_back(action);
        m_recentFilesMenu->addAction(action);
    }
}

void MainWindow::setupUi()
{
    ui->tabWidget->setTabsClosable(true);
    ui->tabWidget->setMovable(true);
    ui->tabWidget->setDocumentMode(true);

    resize(1000, 700);
    connect(ui->tabWidget, &QTabWidget::tabCloseRequested,
        this, [this](int index)
        {
            auto* editor = qobject_cast<EditorWidget*>(ui->tabWidget->widget(index));
            if (!editor)
                return;

            if (!maybeSave(editor))
                return;

            ui->tabWidget->removeTab(index);
            editor->deleteLater();
        });
}

void MainWindow::createNewTab()
{
    auto* editor = new EditorWidget(this);

    QString title = QString("Untitled %1").arg(m_untitledCounter++);
    addEditorTab(editor, title);
}

EditorWidget* MainWindow::currentEditor() const
{
    return qobject_cast<EditorWidget*>(ui->tabWidget->currentWidget());
}

EditorWidget* MainWindow::findOpenEditor(const QString& path)
{
    QString normalized = QFileInfo(path).canonicalFilePath();

    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        auto* editor = qobject_cast<EditorWidget*>(ui->tabWidget->widget(i));
        if (!editor)
            continue;

        QString openPath = QFileInfo(editor->document()->filePath()).canonicalFilePath();

        if (!openPath.isEmpty() && openPath == normalized)
            return editor;
    }

    return nullptr;
}

void MainWindow::openFile()
{
    QString path = QFileDialog::getOpenFileName(this, "Open File");

    if (path.isEmpty())
        return;

    if (auto* existing = findOpenEditor(path))
    {
        ui->tabWidget->setCurrentWidget(existing);
        return;
    }

    auto content = FileManager::readFile(path);
    if (!content)
    {
        QMessageBox::warning(this, "Error", "Could not open file.");
        return;
    }

    auto* editor = new EditorWidget(this);
    editor->setText(*content);
    editor->document()->setFilePath(path);

    addEditorTab(editor, QFileInfo(path).fileName());
    addRecentFile(path);
}

void MainWindow::saveFile()
{
    auto* editor = currentEditor();
    if (!editor)
        return;

    if (editor->document()->isUntitled())
    {
        saveFileAs();
        return;
    }

    QString path = editor->document()->filePath();

    if (!FileManager::writeFile(path, editor->text()))
    {
        QMessageBox::warning(this, "Error", "Could not save file.");
        return;
    }

    editor->document()->setModified(false);
}

void MainWindow::saveFileAs()
{
    auto* editor = currentEditor();
    if (!editor)
        return;

    QString path = QFileDialog::getSaveFileName(this, "Save File As");

    if (path.isEmpty())
        return;

    if (!FileManager::writeFile(path, editor->text()))
    {
        QMessageBox::warning(this, "Error", "Could not save file.");
        return;
    }

    editor->document()->setFilePath(path);
    editor->document()->setModified(false);

    int index = ui->tabWidget->indexOf(editor);
    ui->tabWidget->setTabText(index, QFileInfo(path).fileName());
    addRecentFile(path);

}

void MainWindow::addEditorTab(EditorWidget* editor, const QString& title)
{
    QString user = UserManager::currentUser();
    QFont font = SettingsManager::loadFontPreferences(user);
    bool wrap = SettingsManager::loadWordWrap(user);

    editor->setEditorFont(font);
    editor->setWordWrap(wrap);

    ui->tabWidget->addTab(editor, title);
    ui->tabWidget->setCurrentWidget(editor);

    connect(editor, &EditorWidget::modificationChanged,
        this, [this, editor](bool modified)
        {
            int index = ui->tabWidget->indexOf(editor);
            if (index == -1)
                return;

            QString tabTitle = editor->document()->displayName();
            if (modified)
                tabTitle += "*";

            ui->tabWidget->setTabText(index, tabTitle);
        });
}

bool MainWindow::maybeSave(EditorWidget* editor)
{
    if (!editor->document()->isModified())
        return true;

    QString name = editor->document()->displayName();

    auto ret = QMessageBox::warning(
        this,
        "Unsaved Changes",
        QString("'%1' has unsaved changes.\nDo you want to save?").arg(name),
        QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel
    );

    if (ret == QMessageBox::Save)
    {
        ui->tabWidget->setCurrentWidget(editor);
        saveFile();
        return !editor->document()->isModified();
    }
    else if (ret == QMessageBox::Cancel)
    {
        return false;
    }

    return true; // Discard
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        auto* editor = qobject_cast<EditorWidget*>(ui->tabWidget->widget(i));
        if (!editor)
            continue;

        if (!maybeSave(editor))
        {
            event->ignore();
            return;
        }
    }

    event->accept();
}

void MainWindow::showFindReplaceDialog()
{
    auto* editorWidget = currentEditor();
    if (!editorWidget)
        return;

    if (!m_findDialog)
        m_findDialog = new FindReplaceDialog(this);

    m_findDialog->setEditor(editorWidget->editor());

    m_findDialog->show();
    m_findDialog->raise();
    m_findDialog->activateWindow();
}


void MainWindow::findNext(const QString& text)
{
    auto* editor = currentEditor();
    if (!editor)
        return;

    editor->editor()->find(text);
}

void MainWindow::findPrevious(const QString& text)
{
    auto* editor = currentEditor();
    if (!editor)
        return;

    editor->editor()->find(
        text,
        QTextDocument::FindBackward
    );
}

void MainWindow::replaceOne(
    const QString& find,
    const QString& replace)
{
    auto* editor = currentEditor();
    if (!editor)
        return;

    auto* textEdit = editor->editor();

    if (textEdit->textCursor().hasSelection())
        textEdit->insertPlainText(replace);

    textEdit->find(find);
}

void MainWindow::replaceAll(
    const QString& find,
    const QString& replace)
{
    auto* editor = currentEditor();
    if (!editor)
        return;

    QString content = editor->text();
    content.replace(find, replace, Qt::CaseSensitive);

    editor->setText(content);
}

void MainWindow::addRecentFile(const QString& path)
{
    m_recentFiles.removeAll(path);

    m_recentFiles.prepend(path);

    while (m_recentFiles.size() > MaxRecentFiles)
        m_recentFiles.removeLast();

    SettingsManager::saveRecentFiles(m_recentFiles);

    updateRecentFileActions();
}

void MainWindow::updateRecentFileActions()
{
    int numRecent = qMin(m_recentFiles.size(), MaxRecentFiles);

    for (int i = 0; i < numRecent; ++i)
    {
        QString text = QFileInfo(m_recentFiles[i]).fileName();

        m_recentFileActions[i]->setText(text);
        m_recentFileActions[i]->setData(m_recentFiles[i]);
        m_recentFileActions[i]->setVisible(true);
    }

    for (int i = numRecent; i < MaxRecentFiles; ++i)
    {
        m_recentFileActions[i]->setVisible(false);
    }
}

void MainWindow::openRecentFile()
{
    QAction* action = qobject_cast<QAction*>(sender());

    if (!action)
        return;

    QString path = action->data().toString();

    if (auto* existing = findOpenEditor(path))
    {
        ui->tabWidget->setCurrentWidget(existing);
        return;
    }

    auto content = FileManager::readFile(path);

    if (!content)
    {
        QMessageBox::warning(this, "Error", "Could not open file.");
        return;
    }

    auto* editor = new EditorWidget(this);
    editor->setText(*content);
    editor->document()->setFilePath(path);

    addEditorTab(editor, QFileInfo(path).fileName());
}

void MainWindow::logoutUser()
{
    if (QMessageBox::question(this,
        "Logout",
        "Do you want to logout?")
        != QMessageBox::Yes)
        return;

    UserManager::logout();

    LoginDialog login(this);

    if (login.exec() != QDialog::Accepted)
    {
        close();
        return;
    }

    QString user = UserManager::currentUser();

    // reload preferences
    QFont font = SettingsManager::loadFontPreferences(user);
    bool wrap = SettingsManager::loadWordWrap(user);

    m_customToolbar->setFont(font);
    ui->actionWordWrap->setChecked(wrap);

    // apply settings to open editors
    for (int i = 0; i < ui->tabWidget->count(); ++i)
    {
        auto* editor =
            qobject_cast<EditorWidget*>(ui->tabWidget->widget(i));

        if (!editor) continue;

        editor->setEditorFont(font);
        editor->setWordWrap(wrap);
    }
}

void MainWindow::addAccount()
{
    SignupDialog dialog(this);

    if (dialog.exec() != QDialog::Accepted)
        return;

    logoutUser();
}