#include "DashboardPage.h"
#include "../data/DataProvider.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDateTime>
#include <QGraphicsDropShadowEffect>
#include <QFrame>

static QFrame *card(QWidget *content)
{
  auto *frame = new QFrame;
  frame->setObjectName("Card");
  auto *lay = new QVBoxLayout(frame);
  lay->setContentsMargins(20, 20, 20, 20);
  lay->setSpacing(16);
  lay->addWidget(content);
  auto *shadow = new QGraphicsDropShadowEffect(frame);
  shadow->setBlurRadius(18);
  shadow->setOffset(0, 4);
  shadow->setColor(QColor(0, 0, 0, 28));
  frame->setGraphicsEffect(shadow);
  frame->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  return frame;
}

DashboardPage::DashboardPage(DataProvider *data, QWidget *parent)
    : QWidget(parent), data_(data)
{

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  root->addWidget(buildHeader());
  root->addWidget(buildTable(), 1);

  refresh();
}

QWidget *DashboardPage::buildHeader()
{
  auto *wrap = new QWidget;
  auto *outer = new QVBoxLayout(wrap);
  outer->setContentsMargins(16, 16, 16, 8);
  outer->setSpacing(0);

  auto *content = new QWidget;
  auto *row = new QHBoxLayout(content);
  row->setContentsMargins(0, 0, 0, 0);
  row->setSpacing(16);

  auto *left = new QVBoxLayout;
  auto *title = new QLabel(u8"현재 작업 인원");
  title->setObjectName("SectionTitle");
  realtimeDot_ = new QLabel;
  realtimeDot_->setObjectName("GreenDot");
  realtimeDot_->setFixedSize(10, 10);
  auto *live = new QLabel(u8"실시간");
  live->setObjectName("LiveText");
  auto *liveRow = new QHBoxLayout;
  liveRow->setSpacing(6);
  liveRow->addWidget(realtimeDot_);
  liveRow->addWidget(live);
  liveRow->addStretch(1);
  left->addWidget(title);
  left->addLayout(liveRow);

  auto *mid = new QVBoxLayout;
  auto *countRow = new QHBoxLayout;
  countRow->setContentsMargins(0, 0, 0, 0);
  countLabel_ = new QLabel("0");
  countLabel_->setObjectName("BigCount");
  auto *unit = new QLabel(u8"명");
  unit->setObjectName("UnitText");
  countRow->addStretch(1);
  countRow->addWidget(countLabel_);
  countRow->addWidget(unit);
  countRow->addStretch(1);
  mid->addLayout(countRow);

  auto *right = new QVBoxLayout;
  refreshBtn_ = new QPushButton(u8"새로고침");
  refreshBtn_->setObjectName("RefreshBtn");
  lastUpdatedLabel_ = new QLabel(u8"기준: -");
  lastUpdatedLabel_->setObjectName("UpdatedText");
  right->addWidget(refreshBtn_, 0, Qt::AlignRight);
  right->addWidget(lastUpdatedLabel_, 0, Qt::AlignRight);

  row->addLayout(left, 1);
  row->addLayout(mid, 1);
  row->addLayout(right, 1);

  connect(refreshBtn_, &QPushButton::clicked, this, &DashboardPage::refresh);
  outer->addWidget(card(content));

  return wrap;
}

QWidget *DashboardPage::buildTable()
{
  auto *wrap = new QWidget;
  auto *outer = new QVBoxLayout(wrap);
  outer->setContentsMargins(16, 8, 16, 16);
  outer->setSpacing(8);

  auto *subtitle = new QLabel(u8"현재 작업 인원 (상세)");
  subtitle->setObjectName("SectionTitle");
  outer->addWidget(subtitle, 0, Qt::AlignLeft);

  model_ = new QStandardItemModel(this);
  model_->setColumnCount(5);
  model_->setHorizontalHeaderLabels({u8"번호", u8"시간", u8"이름", u8"연락처", u8"메모"});

  tableView_ = new QTableView;
  tableView_->setModel(model_);
  tableView_->setAlternatingRowColors(true);
  tableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableView_->setSelectionMode(QAbstractItemView::SingleSelection);
  tableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableView_->verticalHeader()->setVisible(false);
  tableView_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  tableView_->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  tableView_->verticalHeader()->setDefaultSectionSize(44);
  tableView_->setShowGrid(false);
  tableView_->setWordWrap(false);

  outer->addWidget(card(tableView_), 1);

  pageInfoLabel_ = new QLabel(u8"총 0건 · 1/1 페이지");
  pageInfoLabel_->setObjectName("PageInfo");
  outer->addWidget(pageInfoLabel_, 0, Qt::AlignLeft);

  return wrap;
}

QString DashboardPage::nowHM() const
{
  return QDateTime::currentDateTime().toString("HH:mm");
}

void DashboardPage::refresh()
{
  // Fill from provider
  const auto rows = data_->currentWorkers();
  model_->removeRows(0, model_->rowCount());
  for (const auto &r : rows)
  {
    QList<QStandardItem *> items;
    items << new QStandardItem(QString::number(r.no))
          << new QStandardItem(r.time)
          << new QStandardItem(r.name)
          << new QStandardItem(r.phoneMasked)
          << new QStandardItem(r.memo);
    for (auto *it : items)
      it->setEditable(false);
    model_->appendRow(items);
  }
  countLabel_->setText(QString::number(data_->currentWorkerCount()));
  lastUpdatedLabel_->setText(u8"기준: 오늘 " + nowHM());
  pageInfoLabel_->setText(u8"총 " + QString::number(rows.size()) + u8"건 · 1/1 페이지");
}
