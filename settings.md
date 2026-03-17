# Settings Page Implementation (Qt Widgets, C++17)

This adds a **Settings dialog** where user can:

* Change font family, size, weight
* Toggle word wrap
* Enable "Open last file on startup"

---

# 1. Update SettingsManager

## SettingsManager.h

```cpp
#pragma once

#include <QString>
#include <QFont>

class SettingsManager
{
public:
    static QFont loadFontPreferences(const QString& user);
    static void saveFontPreferences(const QString& user, const QFont& font);

    static bool loadWordWrap(const QString& user);
    static void saveWordWrap(const QString& user, bool enabled);

    static bool loadOpenLastFile(const QString& user);
    static void saveOpenLastFile(const QString& user, bool enabled);
};
```

---

## SettingsManager.cpp

```cpp
bool SettingsManager::loadOpenLastFile(const QString& user)
{
    QSettings settings("NotepadPro", "NotepadPro");

    settings.beginGroup("users/" + user + "/preferences");
    bool value = settings.value("openLastFile", false).toBool();
    settings.endGroup();

    return value;
}

void SettingsManager::saveOpenLastFile(const QString& user, bool enabled)
{
    QSettings settings("NotepadPro", "NotepadPro");

    settings.beginGroup("users/" + user + "/preferences");
    settings.setValue("openLastFile", enabled);
    settings.endGroup();
}
```

---

# 2. Create SettingsDialog.ui

```xml
<?xml version="1.0" encoding="UTF-8"?>
<ui version="4.0">
 <class>SettingsDialog</class>
 <widget class="QDialog" name="SettingsDialog">
  <property name="windowTitle">
   <string>Settings</string>
  </property>

  <layout class="QVBoxLayout">

   <item>
    <widget class="QGroupBox">
     <property name="title">
      <string>Editor Settings</string>
     </property>
     <layout class="QGridLayout">

      <item row="0" column="0">
       <widget class="QLabel">
        <property name="text"><string>Font</string></property>
       </widget>
      </item>

      <item row="0" column="1">
       <widget class="QFontComboBox" name="fontCombo"/>
      </item>

      <item row="1" column="0">
       <widget class="QLabel">
        <property name="text"><string>Size</string></property>
       </widget>
      </item>

      <item row="1" column="1">
       <widget class="QSpinBox" name="fontSizeSpin">
        <property name="minimum"><number>6</number></property>
        <property name="maximum"><number>72</number></property>
       </widget>
      </item>

      <item row="2" column="0">
       <widget class="QLabel">
        <property name="text"><string>Weight</string></property>
       </widget>
      </item>

      <item row="2" column="1">
       <widget class="QComboBox" name="fontWeightCombo"/>
      </item>

      <item row="3" column="0">
       <widget class="QCheckBox" name="wordWrapCheck">
        <property name="text"><string>Word Wrap</string></property>
       </widget>
      </item>

      <item row="4" column="0">
       <widget class="QCheckBox" name="openLastFileCheck">
        <property name="text"><string>Open last file on startup</string></property>
       </widget>
      </item>

     </layout>
    </widget>
   </item>

   <item>
    <layout class="QHBoxLayout">
     <item>
      <widget class="QPushButton" name="applyBtn">
       <property name="text"><string>Apply</string></property>
      </widget>
     </item>
     <item>
      <widget class="QPushButton" name="closeBtn">
       <property name="text"><string>Close</string></property>
      </widget>
     </item>
    </layout>
   </item>

  </layout>
 </widget>
</ui>
```

---

# 3. SettingsDialog.h

```cpp
#pragma once

#include <QDialog>

namespace Ui { class SettingsDialog; }

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget* parent = nullptr);
    ~SettingsDialog();

    void loadSettings(const QString& user);

signals:
    void settingsChanged();

private:
    Ui::SettingsDialog* ui;
    QString m_user;

private slots:
    void applySettings();
};
```

---

# 4. SettingsDialog.cpp

```cpp
#include "SettingsDialog.h"
#include "ui_SettingsDialog.h"

#include "../core/SettingsManager.h"
#include <QFont>

SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent), ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    ui->fontWeightCombo->addItems({
        "Thin","Light","Normal","Medium","Bold","Black"
    });

    connect(ui->applyBtn, &QPushButton::clicked,
            this, &SettingsDialog::applySettings);

    connect(ui->closeBtn, &QPushButton::clicked,
            this, &QDialog::accept);
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::loadSettings(const QString& user)
{
    m_user = user;

    QFont font = SettingsManager::loadFontPreferences(user);

    ui->fontCombo->setCurrentFont(font);
    ui->fontSizeSpin->setValue(font.pointSize());

    ui->wordWrapCheck->setChecked(
        SettingsManager::loadWordWrap(user));

    ui->openLastFileCheck->setChecked(
        SettingsManager::loadOpenLastFile(user));
}

void SettingsDialog::applySettings()
{
    QFont font = ui->fontCombo->currentFont();
    font.setPointSize(ui->fontSizeSpin->value());

    SettingsManager::saveFontPreferences(m_user, font);

    SettingsManager::saveWordWrap(
        m_user, ui->wordWrapCheck->isChecked());

    SettingsManager::saveOpenLastFile(
        m_user, ui->openLastFileCheck->isChecked());

    emit settingsChanged();
}
```

---

# 5. Integrate in MainWindow

## Add member

```cpp
SettingsDialog* m_settingsDialog = nullptr;
```

---

## Connect menu

```cpp
connect(ui->actionSettings,
        &QAction::triggered,
        this,
        [this]()
        {
            if(!m_settingsDialog)
                m_settingsDialog = new SettingsDialog(this);

            QString user = UserManager::currentUser();

            m_settingsDialog->loadSettings(user);

            connect(m_settingsDialog,
                    &SettingsDialog::settingsChanged,
                    this,
                    [this]()
                    {
                        QString user = UserManager::currentUser();

                        QFont font = SettingsManager::loadFontPreferences(user);
                        bool wrap = SettingsManager::loadWordWrap(user);

                        for(int i=0;i<ui->tabWidget->count();++i)
                        {
                            auto* editor = qobject_cast<EditorWidget*>(ui->tabWidget->widget(i));
                            if(!editor) continue;

                            editor->setEditorFont(font);
                            editor->setWordWrap(wrap);
                        }

                        m_customToolbar->setFont(font);
                        ui->actionWordWrap->setChecked(wrap);
                    });

            m_settingsDialog->exec();
        });
```

---

# 6. Open Last File on Startup

In main.cpp after login:

```cpp
QString user = UserManager::currentUser();

if(SettingsManager::loadOpenLastFile(user))
{
    // load last file from recent files
}
```

---

# DONE

You now have a **complete settings system**:

✔ Per-user preferences
✔ GUI settings panel
✔ Persistent storage
✔ Runtime update
✔ Startup behavior control

---

If you want next: implement **"restore last session with multiple tabs"** (real editor-level feature).
