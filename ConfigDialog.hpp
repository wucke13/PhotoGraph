#pragma once
#include "ConfigPages.hpp"
#include "Typing.hpp"
#include <QDialog>

class QListWidget;
class QListWidgetItem;
class QStackedWidget;

namespace PhotoGraph::config {
class ConfigDialog : public QDialog {
  Q_OBJECT

public:
  ConfigDialog(std::size_t memory);
  ~ConfigDialog();

public slots:
  void changePage(QListWidgetItem* current, QListWidgetItem* previous);

signals:
  void memoryUpdated(std::size_t);

private:
  void createCategories();

  QListWidget* cat_list_;
  QStackedWidget* page_stack_;
  ImageProcessingPage* image_processing_page_;
};
} // namespace PhotoGraph::config
