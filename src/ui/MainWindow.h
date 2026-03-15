#pragma once

#include <QMainWindow>
#include <QStringList>
#include <QVector>

class EditorWidget;
class QAction;
class QMenu;
class FindReplaceDialog;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private:
    Ui::MainWindow* ui;
    int m_untitledCounter = 1;
    static constexpr int MaxRecentFiles = 5;
    QStringList m_recentFiles;
    QMenu* m_recentFilesMenu = nullptr;
    QVector<QAction*> m_recentFileActions;
    FindReplaceDialog* m_findDialog = nullptr;

private:
    void setupMenus();
    void createNewTab();
    void openFile();
    void saveFile();
    void saveFileAs();

    EditorWidget* currentEditor() const;
    EditorWidget* findOpenEditor(const QString& path);

    void addEditorTab(EditorWidget* editor, const QString& title);
    bool maybeSave(EditorWidget* editor);
    void setupUi();
    void updateRecentFileActions();
    void addRecentFile(const QString& path);
    void openRecentFile();

    void showFindReplaceDialog();
    void findNext(const QString& text);
    void findPrevious(const QString& text);
    void replaceOne(const QString& find, const QString& replace);
    void replaceAll(const QString& find, const QString& replace);

protected:
    void closeEvent(QCloseEvent* event) override;
};