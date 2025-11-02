#include "InMemoryDataProvider.h"

InMemoryDataProvider::InMemoryDataProvider(QObject *parent)
    : DataProvider(parent)
{
  workers_ = {
      {1, "14:31:22", u8"김민수", "010-****-****", ""},
      {2, "14:28:30", u8"이세종", "010-****-****", ""},
      {3, "14:21:40", u8"이수연", "010-****-****", ""},
      {4, "14:15:20", u8"박이산", "010-****-****", ""},
      {5, "14:10:42", u8"이순신", "010-****-****", ""}};
  events_ = {
      {1, "14:31:22", u8"김민수", "010-****-****", u8"입실"},
      {2, "14:28:05", u8"박니샨", "010-****-****", u8"출입"},
      {3, "14:21:40", u8"이수연", "010-****-****", u8"입실"}};
}

int InMemoryDataProvider::currentWorkerCount() const { return workers_.size(); }
QVector<WorkerRow> InMemoryDataProvider::currentWorkers() const { return workers_; }
QVector<WorkerRow> InMemoryDataProvider::recentEvents() const { return events_; }
