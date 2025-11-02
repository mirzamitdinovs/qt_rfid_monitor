#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QTableView>
#include <QStandardItemModel>
class DataProvider;

class DashboardPage : public QWidget
{
  Q_OBJECT
public:
  explicit DashboardPage(DataProvider *data, QWidget *parent = nullptr);
private slots:
  void refresh();

private:
  QWidget *buildHeader();
  QWidget *buildTable();

  DataProvider *data_;

  QLabel *realtimeDot_ = nullptr;
  QLabel *countLabel_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;
  QLabel *lastUpdatedLabel_ = nullptr;

  QTableView *tableView_ = nullptr;
  QStandardItemModel *model_ = nullptr;
  QLabel *pageInfoLabel_ = nullptr;
  QString nowHM() const;
};
