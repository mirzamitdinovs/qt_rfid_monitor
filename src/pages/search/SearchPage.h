#pragma once
#include <QWidget>

class QLabel;
class QTableView;
class QStandardItemModel;
class QPushButton;
class DataProvider;

class SearchPage : public QWidget
{
  Q_OBJECT
public:
  explicit SearchPage(DataProvider *data, QWidget *parent = nullptr);

public slots:
  void refresh();

private:
  QWidget *makeHeader();  // title + subtitle
  QWidget *makeFilters(); // tabs + inputs + column toggles
  QWidget *makePreview(); // table + footer

  void buildTable();
  void fillTable();

private:
  DataProvider *data_ = nullptr;

  // UI bits we update
  QTableView *table_ = nullptr;
  QStandardItemModel *model_ = nullptr;
  QLabel *totalLabel_ = nullptr;
  QPushButton *refreshBtn_ = nullptr;

  // column toggles
  bool colTime_ = true;
  bool colEvent_ = true;
  bool colName_ = true;
  bool colPhone_ = true;
  bool colMemo_ = false;
};
