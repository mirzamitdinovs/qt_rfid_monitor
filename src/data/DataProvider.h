#pragma once
#include <QObject>
#include <QString>
#include <QVector>

struct WorkerRow
{
  int no;
  QString time;
  QString name;
  QString phoneMasked;
  QString memo;
};

class DataProvider : public QObject
{
  Q_OBJECT
public:
  using QObject::QObject;
  virtual int currentWorkerCount() const = 0;
  virtual QVector<WorkerRow> currentWorkers() const = 0;

  // (For Search page—stub for now)
  virtual QVector<WorkerRow> recentEvents() const = 0;
};
