#include <QApplication>
#include <QStyleFactory>

#include "mainwindow.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  app.setOrganizationName("shadiy");
  app.setApplicationName("musiqer");

  QApplication::setStyle("fusion");

  MainWindow w;
  w.show();

  return app.exec();
}
