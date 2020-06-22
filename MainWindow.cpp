#include "MainWindow.hpp"
#include "ConfigDialog.hpp"
#include "MagicTricks.hpp"
#include <ImageGraph/core/MemoryDistribution.hpp>
#include <ImageGraph/core/NodeGraph.hpp>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QUrl>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QMenuBar>
#include <qobject.h>
#include <thread>

using namespace PhotoGraph;

MainWindow::MainWindow() {
  if (VIPS_INIT("PhotoGraph")) vips_error_exit(nullptr);
  const int id{qRegisterMetaType<tile_u8_t>()};

  container_->setLayout(v_layout_);

  splitter_->addWidget(view_);
  splitter_->addWidget(preview_);
  v_layout_->addWidget(splitter_);

  QObject::connect(view_, &ImageGraphView::computationStarts, compute_button_,
                   &ComputationButton::distributionFinished);
  QObject::connect(this, &MainWindow::computationFinished, compute_button_, &ComputationButton::computationFinished);
  QObject::connect(compute_button_, &ComputationButton::compute, this, [this]() {
    queue_.apply(graph_);
    emit view_->changesApplied();
    std::thread thread{[&graph{graph_}, memory_limit{memory_limit_.get()}, &view{view_}, this] {
      auto distribution{graph.optimizeMemoryDistribution(memory_limit)};
      emit view->computationStarts();
      std::cout << "start computation!" << std::endl;
      graph.compute(std::move(distribution));
      std::cout << "end computation!" << std::endl;
      emit computationFinished();
    }};
    thread.detach();
  });
  QObject::connect(compute_button_, &ComputationButton::cancel, this, [this]() {
    std::thread thread([this] { graph_.finish(); });
    thread.detach();
  });
  v_layout_->addWidget(compute_button_);

  QObject::connect(save_action_, &QAction::triggered, [this](bool) {
    QUrl home{QUrl::fromLocalFile(QDir::home().absolutePath())};
    auto url{QFileDialog::getSaveFileUrl(this, "Save graph", home, "Graph file (*.graph)")};
    std::cout << "Save: " << url.path().toStdString() << std::endl;
    QFile file{url.path()};
    if (not file.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    file.write(view_->toJson().toJson());
    file.close();
  });
  QObject::connect(load_action_, &QAction::triggered, [this](bool) {
    QUrl home{QUrl::fromLocalFile(QDir::home().absolutePath())};
    auto url{QFileDialog::getOpenFileUrl(this, "Load graph", home, "Graph file (*.graph)")};
    std::cout << "Load: " << url.path().toStdString() << std::endl;
    QFile file{url.path()};
    if (not file.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    view_->fromJson(QJsonDocument::fromJson(file.readAll()));
    file.close();
  });
  QObject::connect(preferences_action_, &QAction::triggered, [this](bool) {
    if (config_dialog_)
      config_dialog_->activateWindow();
    else {
      config_dialog_ = new config::ConfigDialog(memory_limit_.get());
      QObject::connect(config_dialog_, &config::ConfigDialog::memoryUpdated, [this](std::size_t value) {
        memory_limit_.set(value);
        std::cout << "memory_limit: " << value << std::endl;
      });
      QObject::connect(config_dialog_, &QObject::destroyed, [this](QObject* obj) {
        assert(obj == config_dialog_);
        config_dialog_ = nullptr;
      });
      config_dialog_->setAttribute(Qt::WA_DeleteOnClose, true);
      config_dialog_->show();
    }
  });
  file_menu_->addAction(load_action_);
  file_menu_->addAction(save_action_);
  edit_menu_->addAction(preferences_action_);

  setCentralWidget(container_);
  menuBar()->addMenu(file_menu_);
  menuBar()->addMenu(edit_menu_);
  // window.setAcceptDrops(true);
}

MainWindow::~MainWindow() {
  if (config_dialog_) config_dialog_->close();
}
