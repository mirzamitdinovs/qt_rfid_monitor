#pragma once
#include <QObject>
#include <QString>
#include <QVector>

struct WorkerRow
{
  int no;
  QString time; // "HH:mm:ss" or similar
  QString name;
  QString phoneMasked;
  QString memo;
};

class DataProvider : public QObject
{
  Q_OBJECT
public:
  using QObject::QObject;
  virtual ~DataProvider() = default;

  // health status (for auto-fallback)
  virtual bool healthy() const = 0;

  // interview scope
  virtual int currentWorkerCount() const = 0;            // table 1
  virtual QVector<WorkerRow> currentWorkers() const = 0; // table 2

  // extra (Search page preview)
  virtual QVector<WorkerRow> recentEvents() const = 0;
};
