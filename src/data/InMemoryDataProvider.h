#pragma once
#include "DataProvider.h"

class InMemoryDataProvider : public DataProvider
{
  Q_OBJECT
public:
  explicit InMemoryDataProvider(QObject *parent = nullptr);
  int currentWorkerCount() const override;
  QVector<WorkerRow> currentWorkers() const override;
  QVector<WorkerRow> recentEvents() const override;

private:
  QVector<WorkerRow> workers_;
  QVector<WorkerRow> events_;
};
