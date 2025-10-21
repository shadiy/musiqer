#pragma once

#include <QCheckBox>
#include <QDir>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QSettings>
#include <QSplitter>
#include <QStackedWidget>
#include <QStringList>
#include <QTabBar>
#include <QTabWidget>
#include <QTextEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>
#include <qstandardpaths.h>

class MainWindow : public QMainWindow {
  Q_OBJECT
public:
  MainWindow(QWidget *parent = nullptr);

private slots:
  void onSongChanged(const QString &text);
  void onOpenDirectoy();
  void onPlayPressed();

private:
  QMediaPlayer *player;
  QAudioOutput *audioOutput;

  QListWidget *dirs;
  QListWidget *songList;
  QWidget *controls;
  QCheckBox *autoplay;
  QPushButton *playButton;
  QSlider *volume;
  QLabel *elapsed;
  QSlider *playback;
  QLabel *duration;

  QSettings *settings;

  QDir rootDir;
  QDir currentDir;

  bool isUserSeeking = false;

  void loadDirectory(const QString &directoryPath);
};
