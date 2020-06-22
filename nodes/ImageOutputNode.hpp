#pragma once
#include "../ConnectionWrapper.hpp"
#include "../DefaultImageNode.hpp"
#include "../DefaultProxy.hpp"
#include "../MagicTricks.hpp"
#include "../NodeWrapper.hpp"
#include "../SceneSinkNode.hpp"
#include "../TimeIntervals.hpp"
#include <ImageGraph/internal/Typing.hpp>
#include <QtWidgets/QPushButton>
#include <chrono>

namespace PhotoGraph::nodes {
namespace __detail {
class ShowSceneButton : public QPushButton {
  Q_OBJECT

public:
  QJsonObject toJson() const { return {}; }

  ShowSceneButton(QWidget* parent = nullptr) : QPushButton{"Show me!", parent} {}
  ShowSceneButton(QJsonObject, QWidget* parent = nullptr) : ShowSceneButton{parent} {}
};

using ImageOutputProxy = DefaultProxy<ShowSceneButton>;
} // namespace __detail

class ImageOutputNode : public QObject, public DefaultImageNode<__detail::ImageOutputProxy, ImageOutputNode> {
  Q_OBJECT

  using in_port_t = NodeImagePort<PortType::INPUT, ImageGraph::OutNode, ImageOutputNode>;
  using out_port_t = NodeImagePort<PortType::OUTPUT, ImageGraph::OutNode, ImageOutputNode>;
  using parent_t = DefaultImageNode<__detail::ImageOutputProxy, ImageOutputNode>;

  NodeWrapper<ImageGraph::OutNode> converter_;
  NodeWrapper<SceneSinkNode> node_;
  MagicGraphicsScene scene_;

  using time_point_t = time::time_point_t;
  using duration_t = time::duration_t;

  const double epsilon_{0.125};
  time_point_t creation_;
  time::TimeIntervals use_intervals_{};
  void initConnections();
  void showPreviewScene(MagicGraphicsScene* scene);
  void handleApplication();
  void handleComputation();

public:
  static inline QString typeID() { return "ImageOutputNode"; }

  void inputConnectionWillUpdate(in_port_t* port);
  void inputConnectionAdded(in_port_t* port) { inputConnectionUpdated(port); }
  void inputConnectionRemoved(in_port_t* port);
  void inputConnectionUpdated(in_port_t* port);

  void showScene() { showPreviewScene(&scene_); }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  ImageOutputNode(ImageGraphView* view, ActionQueue& queue, QObject* scene_parent = nullptr,
                  QGraphicsItem* parent = nullptr)
      : parent_t{NodeState::MISSING_INPUTS, 1, 0, view, queue, parent},
        converter_{queue}, node_{queue}, scene_{scene_parent}, creation_{time::clock_t::now()} {
    initConnections();
    this->setTitle("Output");
  }
  ImageOutputNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                  QObject* scene_parent = nullptr, QGraphicsItem* parent = nullptr)
      : parent_t{json.value("parent").toObject(), registration, view, queue, parent},
        converter_{queue}, node_{queue}, scene_{scene_parent}, creation_{time::clock_t::now()} {
    initConnections();
  }
  ~ImageOutputNode() { showPreviewScene(nullptr); }
};
} // namespace PhotoGraph::nodes
