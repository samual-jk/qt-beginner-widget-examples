#include "CustomToolbar.h"

CustomToolbar::CustomToolbar(QWidget* parent)
    : QWidget(parent),
    ui(new Ui::CustomToolbarClass)
{
    ui->setupUi(this);

    // Populate font sizes
    QStringList sizes =
    {
        "8","9","10","11","12","14","16","18",
        "20","24","28","36","48","72"
    };

    ui->fontSizeCombo->clear();
    ui->fontSizeCombo->addItems(sizes);

    // Populate weights
    ui->fontWeightCombo->clear();
    ui->fontWeightCombo->addItems({
        "Thin",
        "Light",
        "Normal",
        "Medium",
        "Bold",
        "Black"
        });

    connect(ui->fontComboBox,
        &QFontComboBox::currentFontChanged,
        this,
        &CustomToolbar::updateFont);

    connect(ui->fontSizeCombo,
        &QComboBox::currentTextChanged,
        this,
        &CustomToolbar::updateFont);

    connect(ui->fontWeightCombo,
        &QComboBox::currentTextChanged,
        this,
        &CustomToolbar::updateFont);
}

CustomToolbar::~CustomToolbar()
{
    delete ui;
}

void CustomToolbar::updateFont()
{
    QFont font;
    font.setFamily(ui->fontComboBox->currentFont().family());
    font.setPointSize(ui->fontSizeCombo->currentText().toInt());
    QString weightText = ui->fontWeightCombo->currentText();

    QFont::Weight weight = QFont::Normal;

    if (weightText == "Thin") weight = QFont::Thin;
    else if (weightText == "Light") weight = QFont::Light;
    else if (weightText == "Normal") weight = QFont::Normal;
    else if (weightText == "Medium") weight = QFont::Medium;
    else if (weightText == "Bold") weight = QFont::Bold;
    else if (weightText == "Black") weight = QFont::Black;

    font.setWeight(weight);
    emit fontChanged(font);
}

void CustomToolbar::setFont(const QFont& font)
{
    ui->fontComboBox->setCurrentFont(font);

    ui->fontSizeCombo->setCurrentText(
        QString::number(font.pointSize()));

    int weight = font.weight();
    if (weight <= QFont::Thin)
        ui->fontWeightCombo->setCurrentText("Thin");
    else if (weight <= QFont::Light)
        ui->fontWeightCombo->setCurrentText("Light");
    else if (weight <= QFont::Normal)
        ui->fontWeightCombo->setCurrentText("Normal");
    else if (weight <= QFont::Medium)
        ui->fontWeightCombo->setCurrentText("Medium");
    else if (weight <= QFont::Bold)
        ui->fontWeightCombo->setCurrentText("Bold");
    else
        ui->fontWeightCombo->setCurrentText("Black");
}