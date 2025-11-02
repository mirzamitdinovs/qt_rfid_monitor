#include "SearchPage.h"
#include "../data/DataProvider.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTableView>
#include <QStandardItemModel>
#include <QHeaderView>
#include <QFrame>

static QFrame *card(QWidget *content)
{
  auto *frame = new QFrame;
  frame->setObjectName("Card");
  auto *lay = new QVBoxLayout(frame);
  lay->setContentsMargins(20, 20, 20, 20);
  lay->addWidget(content);
  return frame;
}

SearchPage::SearchPage(DataProvider *data, QWidget *parent)
    : QWidget(parent), data_(data)
{

  auto *root = new QVBoxLayout(this);
  root->setContentsMargins(0, 0, 0, 0);
  root->setSpacing(0);

  // Filters row (skeleton)
  auto *filtersWrap = new QWidget;
  auto *filters = new QVBoxLayout(filtersWrap);
  filters->setContentsMargins(16, 16, 16, 8);
  filters->setSpacing(12);

  auto *title = new QLabel(u8"작업자 진입/출입 검색");
  title->setObjectName("SectionTitle");
  filters->addWidget(title);

  // Placeholder filter controls (we'll wire later)
  auto *row = new QHBoxLayout;
  row->setSpacing(8);
  row->addWidget(new QLabel(u8"기간"));
  row->addStretch(1);
  filters->addLayout(row);

  root->addWidget(card(filtersWrap)); // header card look

  // Preview table
  auto *tableWrap = new QWidget;
  auto *tableL = new QVBoxLayout(tableWrap);
  tableL->setContentsMargins(16, 8, 16, 16);
  tableL->setSpacing(8);

  auto *subtitle = new QLabel(u8"미리보기");
  subtitle->setObjectName("SectionTitle");
  tableL->addWidget(subtitle);

  auto *model = new QStandardItemModel(this);
  model->setColumnCount(5);
  model->setHorizontalHeaderLabels({u8"날짜", u8"시간", u8"이벤트", u8"이름", u8"연락처"});

  auto events = data_->recentEvents();
  for (const auto &e : events)
  {
    QList<QStandardItem *> items;
    items << new QStandardItem(u8"2025.10.01.")
          << new QStandardItem(e.time)
          << new QStandardItem(e.memo) // "입실"/"출입" as demo
          << new QStandardItem(e.name)
          << new QStandardItem(e.phoneMasked);
    for (auto *it : items)
      it->setEditable(false);
    model->appendRow(items);
  }

  auto *table = new QTableView;
  table->setModel(model);
  table->setAlternatingRowColors(true);
  table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table->verticalHeader()->setVisible(false);
  tableL->addWidget(card(table), 1);

  root->addWidget(tableWrap, 1);
}
