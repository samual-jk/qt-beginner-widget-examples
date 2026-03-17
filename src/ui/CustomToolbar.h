#pragma once

#include "ui_CustomToolbar.h"

#include <QWidget>
#include <QFont>

QT_BEGIN_NAMESPACE
namespace Ui { class CustomToolbarClass; }
QT_END_NAMESPACE

class CustomToolbar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomToolbar(QWidget* parent = nullptr);
    ~CustomToolbar();
    void setFont(const QFont& font);

signals:

    void fontChanged(const QFont& font);

private:
    Ui::CustomToolbarClass* ui;

private slots:

    void updateFont();
};