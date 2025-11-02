#pragma once
#include <QWidget>
class DataProvider;

class SearchPage : public QWidget
{
  Q_OBJECT
public:
  explicit SearchPage(DataProvider *data, QWidget *parent = nullptr);

private:
  DataProvider *data_;
};
