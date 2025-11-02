#include "AppWindow.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QEvent>
#include <QScreen>
#include <QStackedWidget>
#include <QPushButton>
#include <QMenuBar>
#include <QStatusBar>
#include <QLabel>
#include <QDebug>

#include "../pages/DashboardPage.h"
#include "../pages/SearchPage.h"
#include "../data/InMemoryDataProvider.h"
#include "../data/MySqlDataProvider.h"

AppWindow::AppWindow(QWidget *parent) : QMainWindow(parent)
{
  if (menuBar())
    menuBar()->hide();
  if (statusBar())
    statusBar()->hide();
  setContentsMargins(0, 0, 0, 0);

  // Try MySQL; fallback to memory so the app always runs
  {
    auto *mysql = new MySqlDataProvider(this);
    if (mysql->healthy())
    {
      qInfo() << "[Provider] Using MySQL provider";
      data_ = mysql;
    }
    else
    {
      qWarning() << "[Provider] MySQL unavailable; falling back to InMemory";
      mysql->deleteLater();
      data_ = new InMemoryDataProvider(this);
    }
  }

  auto *central = new QWidget(this);
  auto *v = new QVBoxLayout(central);
  v->setContentsMargins(0, 0, 0, 0);
  v->setSpacing(0);

  v->addWidget(buildTopNav());

  stack_ = new QStackedWidget;
  stack_->addWidget(new DashboardPage(data_, this)); // index 0
  stack_->addWidget(new SearchPage(data_, this));    // index 1
  v->addWidget(stack_, 1);

  setCentralWidget(central);

  applyNormalConstraints();
}

QWidget *AppWindow::buildTopNav()
{
  auto *nav = new QWidget;
  nav->setObjectName("TopNav");
  auto *h = new QHBoxLayout(nav);
  h->setContentsMargins(16, 8, 16, 8);
  h->setSpacing(8);

  btnDash_ = new QPushButton(u8"현재 작업 인원");
  btnSearch_ = new QPushButton(u8"작업자 진입/출입 검색");

  btnDash_->setProperty("active", true);
  btnSearch_->setProperty("active", false);

  h->addWidget(btnDash_);
  h->addWidget(btnSearch_);
  h->addStretch(1); // keep right side empty (no connection widget)

  connect(btnDash_, &QPushButton::clicked, this, &AppWindow::showDashboard);
  connect(btnSearch_, &QPushButton::clicked, this, &AppWindow::showSearch);
  return nav;
}

void AppWindow::showDashboard()
{
  stack_->setCurrentIndex(0);
  btnDash_->setProperty("active", true);
  btnSearch_->setProperty("active", false);
  style()->unpolish(btnDash_);
  style()->polish(btnDash_);
  style()->unpolish(btnSearch_);
  style()->polish(btnSearch_);
}

void AppWindow::showSearch()
{
  stack_->setCurrentIndex(1);
  btnDash_->setProperty("active", false);
  btnSearch_->setProperty("active", true);
  style()->unpolish(btnDash_);
  style()->polish(btnDash_);
  style()->unpolish(btnSearch_);
  style()->polish(btnSearch_);
}

void AppWindow::applyNormalConstraints() { setMaximumWidth(1200); }
void AppWindow::clearConstraints() { setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX); }

void AppWindow::changeEvent(QEvent *e)
{
  if (e->type() == QEvent::WindowStateChange)
  {
    const auto st = windowState();
    const bool isFs = st.testFlag(Qt::WindowFullScreen);
    const bool isMx = st.testFlag(Qt::WindowMaximized);
    if (isFs || isMx)
    {
      clearConstraints();
    }
    else
    {
      applyNormalConstraints();
      if (auto *s = screen())
      {
        const QRect a = s->availableGeometry();
        const int w = qMin(width(), 1200);
        const int h = qMin(height(), a.height());
        resize(w, h);
        move(a.center() - QPoint(w / 2, h / 2));
      }
    }
  }
  QMainWindow::changeEvent(e);
}
