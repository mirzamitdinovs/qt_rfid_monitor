#include "SearchPage.h"
#include "../../data/DataProvider.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QDate>
#include <QTime>
#include <QFile>
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>
#include <QFrame>
#include <QStyledItemDelegate>
#include <QPainter>

// ───────── helpers ─────────
static QFrame *card(QWidget *inner, int pm = 24)
{
  auto *c = new QFrame;
  c->setObjectName("Card");
  auto *lay = new QVBoxLayout(c);
  lay->setContentsMargins(pm, pm, pm, pm);
  lay->setSpacing(0);
  lay->addWidget(inner);
  return c;
}

class EventBadgeDelegate : public QStyledItemDelegate
{
public:
  using QStyledItemDelegate::QStyledItemDelegate;
  void paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
  {
    QStyleOptionViewItem o(opt);
    initStyleOption(&o, idx);
    const QString txt = o.text;
    const bool isCheckIn = (txt.contains(u8"전입"));  // blue
    const bool isCheckOut = (txt.contains(u8"출입")); // red

    QRect r = o.rect;
    r = r.adjusted(12, 8, -12, -8);

    p->save();
    p->setRenderHint(QPainter::Antialiasing, true);

    QColor bg(232, 241, 255); // blue-ish
    QColor fg(74, 144, 226);
    if (isCheckOut)
    {
      bg = QColor(255, 232, 232);
      fg = QColor(231, 76, 60);
    }

    p->setPen(Qt::NoPen);
    p->setBrush(bg);
    p->drawRoundedRect(r, 6, 6);

    p->setPen(fg);
    p->drawText(r, Qt::AlignCenter, txt);
    p->restore();
  }

  QSize sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const override
  {
    auto s = QStyledItemDelegate::sizeHint(opt, idx);
    s.setHeight(qMax(36, s.height()));
    return s;
  }
};
// ───────────────────────────

SearchPage::SearchPage(DataProvider *data, QWidget *parent)
    : QWidget(parent), data_(data)
{
  // page-scoped QSS
  if (QFile f{":/pages/search/SearchPage.qss"}; f.open(QFile::ReadOnly))
    setStyleSheet(QString::fromUtf8(f.readAll()));

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(20, 20, 20, 20);
  root->setSpacing(20);

  root->addWidget(card(makeHeader(), 24));
  root->addWidget(card(makeFilters(), 24));
  root->addWidget(card(makePreview(), 24), 1);

  refresh();
}

QWidget *SearchPage::makeHeader()
{
  auto *w = new QWidget;
  auto *v = new QVBoxLayout(w);
  v->setContentsMargins(0, 0, 0, 0);
  v->setSpacing(6);

  auto *h1 = new QLabel(u8"작업자 진입/출입 검색");
  h1->setObjectName("PageTitle");
  auto *sub = new QLabel(u8"필터를 적용해 검색할 수 있습니다.");
  sub->setObjectName("PageSubtitle");

  v->addWidget(h1);
  v->addWidget(sub);
  return w;
}

QWidget *SearchPage::makeFilters()
{
  auto *wrap = new QWidget;
  auto *v = new QVBoxLayout(wrap);
  v->setContentsMargins(0, 0, 0, 0);
  v->setSpacing(16);

  // section title
  auto *ft = new QLabel(u8"필터");
  ft->setObjectName("SectionTitle");
  v->addWidget(ft);

  // tabs row
  {
    auto *row = new QHBoxLayout;
    row->setSpacing(8);

    auto mkTab = [&](const QString &text, bool active = false)
    {
      auto *b = new QPushButton(text);
      b->setCheckable(true);
      b->setChecked(active);
      b->setObjectName(active ? "TabActive" : "Tab");
      connect(b, &QPushButton::toggled, [b](bool on)
              {
        b->setObjectName(on ? "TabActive" : "Tab");
        b->style()->unpolish(b);
        b->style()->polish(b); });
      return b;
    };
    row->addWidget(new QLabel(u8"기간"));
    row->addSpacing(8);
    row->addWidget(mkTab(u8"오늘"));
    row->addWidget(mkTab(u8"어제"));
    row->addWidget(mkTab(u8"지난 7일", true));
    row->addWidget(mkTab(u8"지난 30일"));
    row->addStretch(1);
    v->addLayout(row);
  }

  // inputs row
  {
    auto *row = new QHBoxLayout;
    row->setSpacing(16);

    auto mkField = [&](const QString &label, const QString &placeholderOrValue)
    {
      auto *g = new QWidget;
      auto *gv = new QVBoxLayout(g);
      gv->setContentsMargins(0, 0, 0, 0);
      gv->setSpacing(6);
      auto *l = new QLabel(label);
      l->setObjectName("FieldLabel");
      auto *e = new QLineEdit;
      e->setObjectName("Input");
      e->setPlaceholderText(placeholderOrValue);
      if (label == u8"기간")
        e->setText(u8"2025-10-01 ~ 2025-10-14");
      gv->addWidget(l);
      gv->addWidget(e);
      return g;
    };

    row->addWidget(mkField(u8"기간", ""));
    row->addWidget(mkField(u8"구역", u8"전체"));
    row->addWidget(mkField(u8"작업자", u8"전체"));
    row->addWidget(mkField(u8"이벤트", u8"진입, 출입"));

    auto *searchBtn = new QPushButton(u8"검색");
    searchBtn->setObjectName("SearchBtn");
    searchBtn->setMinimumWidth(120);
    row->addWidget(searchBtn);
    row->addStretch(1);
    v->addLayout(row);
  }

  // columns (“포함할 열”)
  {
    auto *bar = new QFrame;
    bar->setObjectName("ColumnsBar");
    auto *h = new QHBoxLayout(bar);
    h->setContentsMargins(16, 10, 16, 10);
    h->setSpacing(24);
    auto *title = new QLabel(u8"포함할 열");
    title->setObjectName("ColumnsTitle");
    h->addWidget(title);

    auto addToggle = [&](const QString &label, bool checked, std::function<void(bool)> cb)
    {
      auto *wrap = new QWidget;
      auto *hh = new QHBoxLayout(wrap);
      hh->setContentsMargins(0, 0, 0, 0);
      hh->setSpacing(8);
      auto *c = new QCheckBox;
      c->setChecked(checked);
      auto *l = new QLabel(label);
      hh->addWidget(c);
      hh->addWidget(l);
      QObject::connect(c, &QCheckBox::toggled, this, [cb](bool on)
                       { cb(on); });
      h->addWidget(wrap);
    };

    addToggle(u8"시간", true, [&](bool on)
              { colTime_  = on;  refresh(); });
    addToggle(u8"이벤트", true, [&](bool on)
              { colEvent_ = on;  refresh(); });
    addToggle(u8"이름", true, [&](bool on)
              { colName_  = on;  refresh(); });
    addToggle(u8"연락처", true, [&](bool on)
              { colPhone_ = on;  refresh(); });
    addToggle(u8"메모", false, [&](bool on)
              { colMemo_  = on;  refresh(); });

    h->addStretch(1);
    v->addWidget(bar);
  }

  return wrap;
}

QWidget *SearchPage::makePreview()
{
  auto *wrap = new QWidget;
  auto *v = new QVBoxLayout(wrap);
  v->setContentsMargins(0, 0, 0, 0);
  v->setSpacing(16);

  auto *title = new QLabel(u8"미리보기");
  title->setObjectName("SectionTitle");
  v->addWidget(title);

  buildTable();
  v->addWidget(table_, 1);

  // footer bar
  auto *foot = new QFrame;
  foot->setObjectName("FooterBar");
  auto *fh = new QHBoxLayout(foot);
  fh->setContentsMargins(20, 14, 20, 14);
  totalLabel_ = new QLabel(u8"총 00건");
  totalLabel_->setObjectName("TotalCount");
  refreshBtn_ = new QPushButton(u8"새로고침");
  refreshBtn_->setObjectName("PrimaryBtn");
  refreshBtn_->setMinimumWidth(180);
  fh->addWidget(totalLabel_);
  fh->addStretch(1);
  fh->addWidget(refreshBtn_);
  v->addWidget(foot);

  connect(refreshBtn_, &QPushButton::clicked, this, &SearchPage::refresh);
  return wrap;
}

void SearchPage::buildTable()
{
  model_ = new QStandardItemModel(this);
  model_->setColumnCount(6);
  model_->setHorizontalHeaderLabels({u8"날짜", u8"시간", u8"이벤트", u8"이름", u8"연락처", u8"메모"});

  table_ = new QTableView;
  table_->setObjectName("DataTable");
  table_->setModel(model_);
  table_->setShowGrid(false);
  table_->setAlternatingRowColors(false);
  table_->setSelectionMode(QAbstractItemView::NoSelection);
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->verticalHeader()->setVisible(false);
  table_->verticalHeader()->setDefaultSectionSize(44);

  auto *hh = table_->horizontalHeader();
  hh->setDefaultAlignment(Qt::AlignLeft | Qt::AlignVCenter);
  hh->setHighlightSections(false);
  hh->setFixedHeight(36);
  hh->setSectionResizeMode(0, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(1, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(2, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(3, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(4, QHeaderView::ResizeToContents);
  hh->setSectionResizeMode(5, QHeaderView::Stretch);

  table_->setItemDelegateForColumn(2, new EventBadgeDelegate(table_));
}

void SearchPage::fillTable()
{
  model_->removeRows(0, model_->rowCount());

  const auto rows = data_ ? data_->recentEvents() : decltype(data_->recentEvents()){};
  const QString today = QDate::currentDate().toString("yy.MM.dd.");

  int total = 0;
  bool flip = false;
  for (const auto &r : rows)
  {
    const QString event = flip ? u8"출입" : u8"전입"; // demo: alternate if your DB doesn’t provide an event field
    flip = !flip;

    QList<QStandardItem *> items;
    items << new QStandardItem(today)         // 날짜
          << new QStandardItem(r.time)        // 시간
          << new QStandardItem(event)         // 이벤트 (badge delegate)
          << new QStandardItem(r.name)        // 이름
          << new QStandardItem(r.phoneMasked) // 연락처
          << new QStandardItem(r.memo);       // 메모
    for (auto *it : items)
      it->setEditable(false);
    model_->appendRow(items);
    ++total;
  }

  // column visibility per toggles
  table_->setColumnHidden(1, !colTime_);
  table_->setColumnHidden(2, !colEvent_);
  table_->setColumnHidden(3, !colName_);
  table_->setColumnHidden(4, !colPhone_);
  table_->setColumnHidden(5, !colMemo_);

  totalLabel_->setText(QString(u8"총 %1건").arg(total));
}

void SearchPage::refresh()
{
  fillTable();
}
