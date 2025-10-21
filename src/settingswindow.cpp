#include "settingswindow.h"

SettingsWindow::SettingsWindow(const QString &currentTheme, QWidget *parent)
    : QDialog(parent) {

  QFormLayout *formLayout = new QFormLayout(this);
  formLayout->setHorizontalSpacing(8);

  QComboBox *themes = new QComboBox(this);
  QDir dir("themes");
  dir.setNameFilters({"*.qss"});
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
  themes->addItems(dir.entryList());
  themes->setCurrentText(currentTheme);
  connect(themes, &QComboBox::currentTextChanged, this,
          [this](const QString &text) { emit themeChanged(text); });
  formLayout->addRow(tr("&Theme"), themes);

  setFixedWidth(300);
  setFixedHeight(300);
  setWindowTitle("Settings");

  setLayout(formLayout);
}
