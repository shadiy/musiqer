#pragma once

#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QHeaderView>
#include <QTableWidget>
#include <QWidget>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qcombobox.h>
#include <qdir.h>
#include <qlabel.h>
#include <qobject.h>
#include <qwidget.h>

class SettingsWindow : public QDialog {
  Q_OBJECT
public:
  SettingsWindow(const QString &currentTheme, QWidget *parent = nullptr);

signals:
  QString themeChanged(const QString &theme);
};
