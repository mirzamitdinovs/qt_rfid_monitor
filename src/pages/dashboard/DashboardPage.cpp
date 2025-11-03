#include "DashboardPage.h"
#include "../../data/DataProvider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QDateTime>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFrame>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QFile>

// ───────────────── helpers ─────────────────
static QFrame *card(QWidget *inner)
{
  auto *c = new QFrame;
  c->setObjectName("Card");
  auto *lay = new QVBoxLayout(c);
  lay->setContentsMargins(24, 24, 24, 24);
  lay->setSpacing(0);
  lay->addWidget(inner);
  return c;
}

class MemoPillDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  void paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
  {
    QStyleOptionViewItem o(opt);
    initStyleOption(&o, idx);
    QRect r = o.rect.adjusted(8, 8, -8, -8);
    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);
    p->setPen(QPen(QColor(0xF0, 0xF0, 0xF0), 1));
    p->setBrush(QColor(0xF9, 0xF9, 0xF9));
    p->drawRoundedRect(r, 6, 6);
    if (!o.text.isEmpty())
    {
      p->setPen(QColor(0x33, 0x33, 0x33));
      p->drawText(r.adjusted(10, 0, -10, 0), Qt::AlignVCenter | Qt::AlignLeft, o.text);
    }
    p->restore();
  }
  QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
  {
    auto s = QStyledItemDelegate::sizeHint(opt, idx);
    s.setHeight(qMax(s.height(), 44));
    return s;
  }
};
// ───────────────────────────────────────────

DashboardPage::DashboardPage(DataProvider *data, QWidget *parent)
    : QWidget(parent), data_(data)
{
  // load module QSS
  if (QFile f{":/pages/dashboard/DashboardPage.qss"}; f.open(QFile::ReadOnly))
  {
    setStyleSheet(QString::fromUtf8(f.readAll()));
  }

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(20, 20, 20, 20);
  root->setSpacing(20);

  // header card
  root->addWidget(card(makeHeaderCard()));

  // table card (with footer inside)
  root->addWidget(card(makeTableCard()), 1);

  refresh();
}

QWidget *DashboardPage::makeHeaderCard()
{
  auto *wrap = new QWidget;
  auto *h = new QHBoxLayout(wrap);
  h->setContentsMargins(0, 0, 0, 0);
  h->setSpacing(24);

  // left: title + realtime
  auto *left = new QWidget;
  auto *lv = new QVBoxLayout(left);
  lv->setContentsMargins(0, 0, 0, 0);
  lv->setSpacing(12);

  auto *title = new QLabel(u8"현재 작업 인원");
  title->setObjectName("HeaderTitle");
  lv->addWidget(title);

  auto *badge = new QWidget;
  badge->setObjectName("StatusBadge");
  auto *bh = new QHBoxLayout(badge);
  bh->setContentsMargins(0, 0, 0, 0);
  bh->setSpacing(6);
  auto *dot = new QLabel;
  dot->setObjectName("StatusDot");
  dot->setFixedSize(6, 6);
  auto *txt = new QLabel(u8"실시간");
  txt->setObjectName("StatusText");
  bh->addWidget(dot);
  bh->addWidget(txt);
  lv->addWidget(badge);
  lv->addStretch(1);

  // center: count box
  auto *center = new QWidget;
  center->setObjectName("HeaderCenter");
  auto *ch = new QHBoxLayout(center);
  ch->setContentsMargins(0, 0, 0, 0);
  ch->setSpacing(0);

  auto *countBox = new QFrame;
  countBox->setObjectName("CountBox");
  auto *cbh = new QHBoxLayout(countBox);
  cbh->setContentsMargins(24, 16, 24, 16);
  cbh->setSpacing(8);

  countLabel_ = new QLabel("0");
  countLabel_->setObjectName("CountNumber");
  auto *unit = new QLabel(u8"명");
  unit->setObjectName("CountUnit");
  cbh->addWidget(countLabel_);
  cbh->addWidget(unit);

  ch->addStretch(1);
  ch->addWidget(countBox, 0, Qt::AlignCenter);
  ch->addStretch(1);

  // right: refresh and timestamp
  auto *right = new QWidget;
  auto *rv = new QVBoxLayout(right);
  rv->setContentsMargins(0, 0, 0, 0);
  rv->setSpacing(8);

  auto *btn = new QPushButton(u8"새로고침");
  btn->setObjectName("RefreshBtn");
  btn->setMinimumWidth(140);
  rv->addWidget(btn, 0, Qt::AlignRight);

  tsLabel_ = new QLabel;
  tsLabel_->setObjectName("Timestamp");
  rv->addWidget(tsLabel_, 0, Qt::AlignRight);
  rv->addStretch(1);

  h->addWidget(left, 1);
  h->addWidget(center, 1);
  h->addWidget(right, 1);

  connect(btn, &QPushButton::clicked, this, &DashboardPage::refresh);
  return wrap;
}

QWidget *DashboardPage::makeTableCard()
{
  auto *wrap = new QWidget;
  auto *v = new QVBoxLayout(wrap);
  v->setContentsMargins(0, 0, 0, 0);
  v->setSpacing(20);

  auto *title = new QLabel(u8"현재 작업 인원 (상세)");
  title->setObjectName("TableTitle");
  v->addWidget(title);

  buildTable();
  v->addWidget(table_, 1);

  auto *foot = new QFrame;
  foot->setObjectName("Pagination");
  auto *fh = new QHBoxLayout(foot);
  fh->setContentsMargins(16, 12, 16, 12);
  fh->addStretch(1);
  footerLabel_ = new QLabel;
  footerLabel_->setObjectName("PaginationText");
  fh->addWidget(footerLabel_, 0, Qt::AlignLeft);
  fh->addStretch(1);
  v->addWidget(foot);

  return wrap;
}

void DashboardPage::buildTable()
{
  model_ = new QStandardItemModel(this);
  model_->setColumnCount(5);
  model_->setHorizontalHeaderLabels({u8"번호", u8"시간", u8"이름", u8"연락처", u8"메모"});

  table_ = new QTableView;
  table_->setObjectName("DataTable");
  table_->setModel(model_);
  table_->setShowGrid(false);
  table_->setAlternatingRowColors(false);
  table_->setSelectionMode(QAbstractItemView::NoSelection);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->verticalHeader()->setVisible(false);
  table_->verticalHeader()->setDefaultSectionSize(48);

  auto *hh = table_->horizontalHeader();
  hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  hh->setHighlightSections(false);
  hh->setFixedHeight(36);
  hh->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(4, QHeaderView::Stretch);

  table_->setItemDelegateForColumn(4, new MemoPillDelegate(table_));
}

void DashboardPage::fillTable()
{
  model_->removeRows(0, model_->rowCount());

  const auto rows = data_ ? data_->currentWorkers() : decltype(data_->currentWorkers()){};
  int total = 0;
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
    ++total;
  }

  // count from DB; fallback to table size
  const int cnt = data_ ? data_->currentWorkerCount() : total;
  countLabel_->setText(QString::number(cnt));

  // timestamp like "기준: 오늘 HH:MM"
  tsLabel_->setText(QString(u8"기준: 오늘 %1").arg(QTime::currentTime().toString("HH:mm")));

  footerLabel_->setText(QString(u8"총 %1건 · 1/1 페이지").arg(model_->rowCount()));
}

void DashboardPage::refresh()
{
  fillTable();
}
