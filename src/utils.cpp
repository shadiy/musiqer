#include "utils.h"

std::string toStdStringSafe(const QString &s) {
  QByteArray arr = s.toUtf8();
  return std::string(arr.constData() ? arr.constData() : "", arr.size());
};
