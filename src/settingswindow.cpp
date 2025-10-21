#include "settingswindow.h"
#include <qboxlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qwidget.h>

SettingsWindow::SettingsWindow(const QString &currentTheme,
                               const QString &currentDir, QWidget *parent)
    : QDialog(parent) {

  QFormLayout *formLayout = new QFormLayout(this);
  formLayout->setHorizontalSpacing(8);

  QWidget *currentDirInputWidget = new QWidget(this);
  QVBoxLayout *dirLayout = new QVBoxLayout(currentDirInputWidget);
  dirLayout->setContentsMargins(0, 0, 0, 0);
  QLabel *currentDirLabel = new QLabel(currentDir, currentDirInputWidget);
  QPushButton *changeDirButton = new QPushButton(currentDirInputWidget);
  changeDirButton->setText("Change");
  connect(changeDirButton, &QPushButton::pressed, this,
          [this, currentDirLabel, currentDir]() {
            QString directoryPath = QFileDialog::getExistingDirectory(
                nullptr, "Select Directory", currentDir);
            currentDirLabel->setText(directoryPath);
            emit directoryChanged(directoryPath);
          });
  dirLayout->addWidget(currentDirLabel);
  dirLayout->addWidget(changeDirButton);
  formLayout->addRow(tr("Directory"), currentDirInputWidget);

  QComboBox *themes = new QComboBox(this);
  QDir dir("themes");
  dir.setNameFilters({"*.qss"});
  dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
  themes->addItems(dir.entryList());
  themes->setCurrentText(currentTheme);
  connect(themes, &QComboBox::currentTextChanged, this,
          [this](const QString &text) { emit themeChanged(text); });
  formLayout->addRow(tr("Theme"), themes);

  setFixedWidth(300);
  setFixedHeight(300);
  setWindowTitle("Settings");

  setLayout(formLayout);
}
