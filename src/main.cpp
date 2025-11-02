#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QFontDatabase>
#include <QFile>     // ← add
#include <QIODevice> // ← add
#include "app/AppWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // Load Korean font
  QFontDatabase::addApplicationFont(":/fonts/NotoSansKR-Regular.ttf");
  app.setFont(QFont("Noto Sans CJK KR, Noto Sans KR, Malgun Gothic, Apple SD Gothic Neo, Nanum Gothic", 11));

  // Global QSS
  QFile f(":/styles/app.qss");
  if (f.open(QIODevice::ReadOnly))
  {
    app.setStyleSheet(QString::fromUtf8(f.readAll()));
    f.close();
  }

  AppWindow w;

  // Start in centered normal size (≤1200 x 800)
  const QRect avail = QGuiApplication::primaryScreen()->availableGeometry();
  const int targetW = qMin(1200, avail.width() - 2);
  const int targetH = qMin(800, avail.height() - 2);
  w.resize(targetW, targetH);
  w.move(avail.center() - QPoint(targetW / 2, targetH / 2));
  w.show();

  return app.exec();
}
