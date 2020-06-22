#pragma once
#include "ActionQueue.hpp"
#include "ComputationButton.hpp"
#include "ConfigDialog.hpp"
#include "ImageGraphView.hpp"
#include "MagicTricks.hpp"
#include "QSetting.hpp"
#include <QNodeGraph/NodeGraphScene.hpp>
#include <QSettings>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QWidget>
#include <type_traits>

namespace PhotoGraph {
class MainWindow : public QMainWindow {
  Q_OBJECT

  template<typename T> T getDefault(const QString& key, T def) {
    auto res{settings.value(key)};
    if (res.canConvert<T>())
      return res.value<T>();
    else {
      settings.setValue(key, def);
      return std::move(def);
    }
  }
  static_assert(decltype(qulonglong{std::declval<std::size_t>()}, std::true_type{}){});

  QSettings settings{"MagpieComputing", "PhotoGraph"};
  config::QSetting<qulonglong> memory_limit_{settings, "memory_limit", 100'000'000};
  ImageGraph::NodeGraph graph_{};
  ActionQueue queue_{};
  NodeGraphScene* scene_{new NodeGraphScene};
  QGraphicsView* preview_{new QGraphicsView};
  ImageGraphView* view_{new ImageGraphView(queue_, preview_, scene_)};
  QWidget* container_{new QWidget};
  QVBoxLayout* v_layout_{new QVBoxLayout};
  QSplitter* splitter_{new QSplitter{this}};
  ComputationButton* compute_button_{new ComputationButton()};
  QMenu* file_menu_{new QMenu{"File"}};
  QAction* save_action_{new QAction{"Save"}};
  QAction* load_action_{new QAction{"Load"}};
  QMenu* edit_menu_{new QMenu{"Edit"}};
  QAction* preferences_action_{new QAction{"Preferences"}};
  config::ConfigDialog* config_dialog_{nullptr};

signals:
  void computationFinished();

public:
  MainWindow();
  ~MainWindow();
};
} // namespace PhotoGraph
