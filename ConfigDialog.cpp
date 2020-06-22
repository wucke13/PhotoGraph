#include <QtWidgets>
#include <iostream>

#include "ConfigDialog.hpp"
#include "ConfigPages.hpp"

using namespace PhotoGraph::config;

ConfigDialog::ConfigDialog(std::size_t memory)
    : cat_list_{new QListWidget}, page_stack_{new QStackedWidget}, image_processing_page_{
                                                                       new ImageProcessingPage(memory)} {
  cat_list_->setMovement(QListView::Static);
  cat_list_->setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);
  cat_list_->setSizeAdjustPolicy(QListWidget::AdjustToContents);

  connect(image_processing_page_, &ImageProcessingPage::memoryUpdated, this, [this](std::size_t value) {
    std::cout << "ConfigDialog memoryUpdated" << std::endl;
    emit memoryUpdated(value);
  });
  page_stack_->addWidget(image_processing_page_);

  QPushButton* close_button{new QPushButton(tr("Close"))};

  createCategories();
  cat_list_->setCurrentRow(0);

  connect(close_button, &QAbstractButton::clicked, this, &QWidget::close);

  QHBoxLayout* upper_layout{new QHBoxLayout};
  upper_layout->addWidget(cat_list_);
  upper_layout->addWidget(page_stack_, 1);

  QHBoxLayout* lower_layout{new QHBoxLayout};
  lower_layout->addStretch(1);
  lower_layout->addWidget(close_button);

  QVBoxLayout* vertical_layout{new QVBoxLayout};
  vertical_layout->addLayout(upper_layout);
  vertical_layout->addLayout(lower_layout);
  setLayout(vertical_layout);

  setWindowTitle(tr("Configuration"));
}

ConfigDialog::~ConfigDialog() { std::cout << "~ConfigDialog" << std::endl; }

void ConfigDialog::createCategories() {
  QListWidgetItem* image_proc{new QListWidgetItem(cat_list_)};
  image_proc->setText(tr("Image Processing"));
  image_proc->setTextAlignment(Qt::AlignCenter);
  image_proc->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

  // Add more buttons here if necessary!

  connect(cat_list_, &QListWidget::currentItemChanged, this, &ConfigDialog::changePage);
}

void ConfigDialog::changePage(QListWidgetItem* current, QListWidgetItem* previous) {
  if (not current) current = previous;
  page_stack_->setCurrentIndex(cat_list_->row(current));
}