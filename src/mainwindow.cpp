#include "mainwindow.h"
#include <qapplication.h>
#include <qboxlayout.h>
#include <qglobal.h>
#include <qhashfunctions.h>
#include <qlayoutitem.h>
#include <qlistwidget.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qtoolbutton.h>
#include <qtypes.h>
#include <qwidget.h>

#include "settingswindow.h"

QString fromMicroSecToMinSec(qint64 value) {
  value = value / 1000;
  int minutes = value / 60;
  int seconds = value % 60;

  return QString("%1:%2")
      .arg(minutes, 2, 10, QLatin1Char('0'))
      .arg(seconds, 2, 10, QLatin1Char('0'));
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  settings = new QSettings();

  QString theme = settings->value("theme", "dracula.qss").toString();
  QFile f("themes/" + theme);
  if (f.open(QFile::ReadOnly | QFile::Text)) {
    qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
    f.close();
  }

  player = new QMediaPlayer;

  connect(player, &QMediaPlayer::positionChanged, this,
          [this](qint64 position) {
            if (isUserSeeking)
              return;

            playback->setValue(static_cast<int>(position));
            elapsed->setText(fromMicroSecToMinSec(position));
          });

  connect(player, &QMediaPlayer::mediaStatusChanged, this,
          [this](QMediaPlayer::MediaStatus status) {
            if (status == QMediaPlayer::EndOfMedia) {
              if (autoplay->isChecked()) {
                int currentRow = songList->currentRow() + 1;
                if (currentRow >= songList->count()) {
                  currentRow = 0;
                }
                songList->setCurrentRow(currentRow);
              } else {
                player->setPosition(0);
                player->play();
              }
            }
          });

  connect(player, &QMediaPlayer::durationChanged, this,
          [this](qint64 duration) {
            playback->setMaximum(static_cast<int>(duration));
            this->duration->setText(fromMicroSecToMinSec(duration));
          });

  audioOutput = new QAudioOutput;
  player->setAudioOutput(audioOutput);

  QWidget *main = new QWidget(this);

  QWidget *top = new QWidget(main);
  QHBoxLayout *topLayout = new QHBoxLayout(top);
  topLayout->setContentsMargins(0, 0, 0, 0);

  QWidget *sidebar = new QWidget(top);
  QVBoxLayout *sidebarLayout = new QVBoxLayout(sidebar);
  sidebarLayout->setContentsMargins(0, 0, 0, 0);

  QPushButton *openSettings = new QPushButton(sidebar);
  openSettings->setText("Open Settings");
  connect(openSettings, &QPushButton::pressed, this, [this]() {
    SettingsWindow *settingsWindow =
        new SettingsWindow(settings->value("theme", "dracula.qss").toString(),
                           rootDir.absolutePath(), this);

    connect(settingsWindow, &SettingsWindow::themeChanged, this,
            [this](const QString &theme) {
              QFile f("themes/" + theme);
              if (f.open(QFile::ReadOnly | QFile::Text)) {
                qApp->setStyleSheet(QString::fromUtf8(f.readAll()));
                f.close();
                settings->setValue("theme", theme);
              }
            });

    connect(settingsWindow, &SettingsWindow::directoryChanged, this,
            &MainWindow::loadDirectory);

    settingsWindow->setAttribute(Qt::WA_ShowModal);
    settingsWindow->setAttribute(Qt::WA_DeleteOnClose);
    settingsWindow->exec();
  });
  sidebarLayout->addWidget(openSettings);

  dirs = new QListWidget(top);
  dirs->setContentsMargins(0, 0, 0, 0);

  connect(dirs, &QListWidget::currentTextChanged, this,
          [this](const QString &text) {
            songList->clear();

            if (text == rootDir.dirName()) {
              currentDir = rootDir;
            } else {
              currentDir = rootDir.filePath(text);
            }

            currentDir.setNameFilters({"*.mp3"});
            currentDir.setFilter(QDir::NoDotAndDotDot | QDir::Files);

            QStringList files = currentDir.entryList();
            for (const QString &file : files) {
              songList->addItem(file);
            }
          });
  sidebarLayout->addWidget(dirs);

  songList = new QListWidget(top);
  songList->setSelectionMode(QAbstractItemView::SingleSelection);
  songList->setContentsMargins(0, 0, 0, 0);

  connect(songList, &QListWidget::currentTextChanged, this,
          &MainWindow::onSongChanged);

  topLayout->addWidget(sidebar);
  topLayout->addWidget(songList, 1);

  //

  controls = new QWidget(main);

  QVBoxLayout *controlsLayout = new QVBoxLayout(controls);
  controlsLayout->setContentsMargins(8, 8, 8, 8);

  QWidget *firstControls = new QWidget(controls);

  QHBoxLayout *firstControlsLayout = new QHBoxLayout(firstControls);
  firstControlsLayout->setContentsMargins(0, 0, 0, 0);

  autoplay = new QCheckBox("Autoplay", firstControls);
  autoplay->setChecked(settings->value("autoplay", true).toBool());

  //

  QPushButton *prev = new QPushButton(firstControls);
  prev->setIcon(
      QApplication::style()->standardIcon(QStyle::SP_MediaSeekBackward));
  prev->setObjectName("play");

  connect(prev, &QPushButton::pressed, this, [this]() {
    int currentRow = songList->currentRow() - 1;
    if (currentRow < 0) {
      currentRow = 0;
    }
    songList->setCurrentRow(currentRow);
  });

  playButton = new QPushButton(firstControls);
  playButton->setIcon(
      QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
  playButton->setObjectName("play");

  connect(playButton, &QPushButton::pressed, this, &MainWindow::onPlayPressed);

  QPushButton *forv = new QPushButton(firstControls);
  forv->setIcon(
      QApplication::style()->standardIcon(QStyle::SP_MediaSeekForward));
  forv->setObjectName("play");

  connect(forv, &QPushButton::pressed, this, [this]() {
    int currentRow = songList->currentRow() + 1;
    if (currentRow >= songList->count()) {
      currentRow = 0;
    }
    songList->setCurrentRow(currentRow);
  });

  //

  volume = new QSlider(Qt::Horizontal);
  volume->setObjectName("VolumeSlider");
  volume->setMinimum(0);
  volume->setMaximum(100);
  volume->setValue(50);
  volume->setMaximumWidth(200);

  connect(volume, &QSlider::valueChanged,
          [this](int value) { audioOutput->setVolume((float)value / 100); });

  firstControlsLayout->addWidget(autoplay);
  firstControlsLayout->addWidget(prev);
  firstControlsLayout->addWidget(playButton);
  firstControlsLayout->addWidget(forv);
  firstControlsLayout->addWidget(volume);
  firstControlsLayout->setAlignment(Qt::AlignHCenter);

  //

  QWidget *secondControls = new QWidget(controls);
  QHBoxLayout *secondLayout = new QHBoxLayout(secondControls);
  secondLayout->setContentsMargins(0, 0, 0, 0);

  elapsed = new QLabel(secondControls);
  elapsed->setText("00:00");

  playback = new QSlider(Qt::Horizontal);
  playback->setMinimum(0);
  playback->setMaximum(100);
  playback->setValue(50);

  connect(playback, &QSlider::sliderPressed, this,
          [this]() { isUserSeeking = true; });

  connect(playback, &QSlider::sliderReleased, this, [this]() {
    player->setPosition(playback->value());
    isUserSeeking = false;
  });

  connect(playback, &QSlider::sliderMoved, this, [this](int value) {
    if (isUserSeeking)
      player->setPosition(value);
  });

  duration = new QLabel(secondControls);
  duration->setText("00:00");

  secondLayout->addWidget(elapsed);
  secondLayout->addWidget(playback);
  secondLayout->addWidget(duration);

  controlsLayout->addWidget(secondControls);
  controlsLayout->addWidget(firstControls);

  //

  QVBoxLayout *mainLayout = new QVBoxLayout(main);
  mainLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->addWidget(top);
  mainLayout->addWidget(controls);

  main->setLayout(mainLayout);

  //

  int pos_x = settings->value("x", x()).toInt();
  int pos_y = settings->value("y", y()).toInt();
  int size_width = settings->value("width", 1280).toInt();
  int size_height = settings->value("height", 720).toInt();

  setCentralWidget(main);
  move(pos_x, pos_y);
  resize(size_width, size_height);
  setWindowTitle("musiqer");

  QString qlastDir = settings
                         ->value("last_dir", QStandardPaths::writableLocation(
                                                 QStandardPaths::MusicLocation))
                         .toString();
  if (!qlastDir.isEmpty()) {
    loadDirectory(qlastDir);
  }
}

void MainWindow::onOpenDirectoy() {}

void MainWindow::loadDirectory(const QString &directoryPath) {
  rootDir = QDir(directoryPath);

  settings->setValue("last_dir", directoryPath);

  dirs->clear();

  dirs->addItem(rootDir.dirName());

  rootDir.setFilter(QDir::NoDotAndDotDot | QDir::Dirs);

  QStringList newdirs = rootDir.entryList();
  for (const QString &dir : newdirs) {
    dirs->addItem(dir);
  }
}

void MainWindow::onSongChanged(const QString &text) {
  player->setSource(QUrl::fromLocalFile(currentDir.filePath(text)));

  playback->setMaximum(player->duration());

  audioOutput->setVolume((float)volume->value() / 100);
  player->play();

  playButton->setIcon(
      QApplication::style()->standardIcon(QStyle::SP_MediaPause));
}

void MainWindow::onPlayPressed() {
  if (player->isPlaying()) {
    player->pause();
    playButton->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_MediaPlay));
  } else {
    player->play();
    playButton->setIcon(
        QApplication::style()->standardIcon(QStyle::SP_MediaPause));
  }
}
