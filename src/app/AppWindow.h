#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>

class DataProvider;

class AppWindow : public QMainWindow
{
  Q_OBJECT
public:
  explicit AppWindow(QWidget *parent = nullptr);

protected:
  void changeEvent(QEvent *e) override;

private slots:
  void showDashboard();
  void showSearch();

private:
  QWidget *buildTopNav();
  void applyNormalConstraints();
  void clearConstraints();

  QStackedWidget *stack_ = nullptr;
  QPushButton *btnDash_ = nullptr;
  QPushButton *btnSearch_ = nullptr;

  DataProvider *data_ = nullptr; // owned
};
