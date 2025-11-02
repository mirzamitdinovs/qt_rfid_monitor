#include <QApplication>
#include <QFontDatabase>
#include "app/AppWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);

  // ✅ Load embedded Korean font
  const int id = QFontDatabase::addApplicationFont(":/fonts/NotoSansKR-Regular.ttf");
  if (id >= 0)
  {
    const auto fam = QFontDatabase::applicationFontFamilies(id).value(0);
    if (!fam.isEmpty())
      QApplication::setFont(QFont(fam));
  }

  // ✅ AppWindow creates its own MySqlDataProvider or fallback internally
  AppWindow w;
  w.resize(1200, 800);
  w.show();

  return app.exec();
}
