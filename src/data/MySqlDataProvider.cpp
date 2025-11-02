#include "MySqlDataProvider.h"
#include "../db/DbConfig.h"

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlRecord>
#include <QVariant>
#include <QDebug>

static const char *kConnName = "rfid_conn";

QString MySqlDataProvider::err2(const QSqlError &e)
{
  return QStringLiteral("%1 (%2)").arg(e.text(), e.nativeErrorCode());
}

MySqlDataProvider::MySqlDataProvider(QObject *parent)
    : DataProvider(parent)
{
  // Ensure driver exists
  if (!QSqlDatabase::isDriverAvailable("QMYSQL"))
  {
    qWarning() << "[DB] QMYSQL driver not available";
    healthy_ = false;
    emit connectionStateChanged(false);
    return;
  }

  // Create/attach connection
  db_ = QSqlDatabase::contains(kConnName)
            ? QSqlDatabase::database(kConnName)
            : QSqlDatabase::addDatabase("QMYSQL", kConnName);

  db_.setHostName(DB_HOST);
  db_.setPort(DB_PORT);
  db_.setUserName(DB_USER);
  db_.setPassword(DB_PASSWORD);
  db_.setDatabaseName(DB_NAME);

  // Keep reasonable timeouts; avoid deprecated MYSQL_OPT_RECONNECT.
  // Supported options differ by driver; these are common with MySQL 8.
  db_.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=5;MYSQL_OPT_READ_TIMEOUT=5;MYSQL_OPT_WRITE_TIMEOUT=5");

  if (!db_.open())
  {
    qWarning() << "[DB] open failed:" << err2(db_.lastError());
    healthy_ = false;
    emit connectionStateChanged(false);
    return;
  }

  // Quick ping
  {
    QSqlQuery ping(db_);
    if (!ping.exec("SELECT 1"))
    {
      qWarning() << "[DB] ping failed:" << err2(ping.lastError());
      healthy_ = false;
      emit connectionStateChanged(false);
      return;
    }
  }

  // Ensure required tables/seed exist so UI never breaks on 1146 again
  if (!ensureSchema())
  {
    healthy_ = false;
    emit connectionStateChanged(false);
    return;
  }

  healthy_ = true;
  emit connectionStateChanged(true);
}

MySqlDataProvider::~MySqlDataProvider()
{
  // Close and remove connection safely
  if (db_.isValid())
  {
    const QString name = db_.connectionName();
    db_.close();
    // Important: make a copy of the connection name and ensure 'db_' goes out of scope first
    QSqlDatabase::removeDatabase(name);
  }
}

bool MySqlDataProvider::ensureSchema()
{
  // Create current_workers if missing
  {
    QSqlQuery q(db_);
    if (!q.exec(R"SQL(
      CREATE TABLE IF NOT EXISTS current_workers (
        id INT PRIMARY KEY,
        count_value INT NOT NULL,
        updated_at TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
          ON UPDATE CURRENT_TIMESTAMP
      )
    )SQL"))
    {
      qWarning() << "[DB] ensureSchema: create current_workers failed:" << err2(q.lastError());
      return false;
    }
  }

  // Create area_workers if missing
  {
    QSqlQuery q(db_);
    if (!q.exec(R"SQL(
      CREATE TABLE IF NOT EXISTS area_workers (
        id INT AUTO_INCREMENT PRIMARY KEY,
        check_time DATETIME NOT NULL DEFAULT CURRENT_TIMESTAMP,
        name VARCHAR(64) NOT NULL,
        phone_masked VARCHAR(32) NOT NULL,
        memo TEXT
      )
    )SQL"))
    {
      qWarning() << "[DB] ensureSchema: create area_workers failed:" << err2(q.lastError());
      return false;
    }
  }

  // Guarantee there is at least one snapshot row in current_workers
  {
    QSqlQuery q(db_);
    if (!q.exec("SELECT COUNT(*) FROM current_workers"))
    {
      qWarning() << "[DB] ensureSchema: count current_workers failed:" << err2(q.lastError());
      return false;
    }
    int rows = 0;
    if (q.next())
      rows = q.value(0).toInt();

    if (rows == 0)
    {
      QSqlQuery ins(db_);
      if (!ins.exec("INSERT INTO current_workers (id, count_value) VALUES (1, 0)"))
      {
        qWarning() << "[DB] ensureSchema: seed current_workers failed:" << err2(ins.lastError());
        return false;
      }
    }
  }

  return true;
}

int MySqlDataProvider::currentWorkerCount() const
{
  if (!healthy_)
    return 0;

  QSqlQuery q(db_);
  // interview requirement: table holds the single integer (id=1 recommended)
  if (!q.exec("SELECT count_value FROM current_workers ORDER BY updated_at DESC LIMIT 1"))
  {
    qWarning() << "[DB] currentWorkerCount failed:" << err2(q.lastError());
    return 0;
  }
  if (q.next())
    return q.value(0).toInt();
  return 0;
}

QVector<WorkerRow> MySqlDataProvider::currentWorkers() const
{
  QVector<WorkerRow> rows;
  if (!healthy_)
    return rows;

  QSqlQuery q(db_);
  if (!q.exec(R"SQL(
      SELECT id,
             DATE_FORMAT(check_time, '%H:%i:%s') AS hhmmss,
             name,
             phone_masked,
             COALESCE(memo,'')
      FROM area_workers
      ORDER BY id ASC
  )SQL"))
  {
    qWarning() << "[DB] currentWorkers failed:" << err2(q.lastError());
    return rows;
  }

  while (q.next())
  {
    WorkerRow r;
    r.no = q.value(0).toInt();
    r.time = q.value(1).toString();
    r.name = q.value(2).toString();
    r.phoneMasked = q.value(3).toString();
    r.memo = q.value(4).toString();
    rows.push_back(r);
  }
  return rows;
}

QVector<WorkerRow> MySqlDataProvider::recentEvents() const
{
  // For now just mirror currentWorkers; later you can point this to a movements table.
  return currentWorkers();
}
