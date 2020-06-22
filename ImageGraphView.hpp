#pragma once

#include "ActionQueue.hpp"
#include <QNodeGraph/NodeGraphScene.hpp>
#include <QNodeGraph/NodeGraphView.hpp>
#include <QtCore/QJsonDocument>
#include <QtWidgets/QGraphicsScene>

class ImageGraphView : public NodeGraphView {
  Q_OBJECT
  NodeGraphScene scene_;
  ActionQueue& queue_;
  QGraphicsView* preview_;

  template<typename T> void addTypedNode(QString type_name, QStringList categories) {
    addNodeType(
        T::typeID(), type_name, categories, [this] { return new T(this, queue_); },
        [this](QJsonObject object, PortRegistration& reg) { return new T(object, reg, this, queue_); });
  }

  template<typename T> void addTypedConnection() {
    addConnectionType(T::typeID(), [this](QJsonObject object, const PortStorage& storage) {
      std::string string{QJsonDocument{object}.toJson().toStdString()};
      return new T(object, storage, this->nodeGraphScene());
    });
  }

signals:
  void changesApplied();
  void computationStarts();
  void previewSceneChanges(QGraphicsScene* old_scene, QGraphicsScene* new_scene);

public:
  ImageGraphView(ActionQueue& queue, QGraphicsView* preview, NodeGraphScene* scene, QWidget* parent = nullptr);

  void setPreviewScene(QGraphicsScene* scene) {
    emit previewSceneChanges(preview_->scene(), scene);
    preview_->setScene(scene);
  }
};
