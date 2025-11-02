#pragma once
#include "DataProvider.h"

class InMemoryDataProvider : public DataProvider
{
  Q_OBJECT
public:
  explicit InMemoryDataProvider(QObject *parent = nullptr);
  bool healthy() const override { return true; }

  int currentWorkerCount() const override;
  QVector<WorkerRow> currentWorkers() const override;
  QVector<WorkerRow> recentEvents() const override;

private:
  QVector<WorkerRow> workers_;
  QVector<WorkerRow> events_;
};
