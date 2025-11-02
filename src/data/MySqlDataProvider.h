// src/data/MySqlDataProvider.h
#pragma once
#include "DataProvider.h"

// ✅ use the module-qualified include
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>

class MySqlDataProvider : public DataProvider
{
  Q_OBJECT
public:
  explicit MySqlDataProvider(QObject *parent = nullptr);
  ~MySqlDataProvider() override;

  bool healthy() const override { return healthy_; }

  int currentWorkerCount() const override;
  QVector<WorkerRow> currentWorkers() const override;
  QVector<WorkerRow> recentEvents() const override;

signals:
  void connectionStateChanged(bool ok);

private:
  bool ensureSchema();
  static QString err2(const QSqlError &e);

private:
  bool healthy_ = false;
  QSqlDatabase db_;
};
