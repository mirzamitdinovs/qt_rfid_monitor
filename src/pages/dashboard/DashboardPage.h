#pragma once
#include <QWidget>

class QLabel;
class QTableView;
class QStandardItemModel;
class DataProvider;

class DashboardPage : public QWidget
{
  Q_OBJECT
public:
  explicit DashboardPage(DataProvider *data, QWidget *parent = nullptr);

public slots:
  void refresh();

private:
  void buildHeader();
  void buildTable();
  void fillTable();
  QWidget *makeHeaderCard();
  QWidget *makeTableCard();

private:
  DataProvider *data_ = nullptr;

  // header
  QLabel *countLabel_ = nullptr;
  QLabel *tsLabel_ = nullptr;

  // table
  QTableView *table_ = nullptr;
  QStandardItemModel *model_ = nullptr;

  // footer
  QLabel *footerLabel_ = nullptr;
};
