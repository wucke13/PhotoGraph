#pragma once
#include "../ConnectionWrapper.hpp"
#include "../DefaultImageNode.hpp"
#include "../DefaultProxy.hpp"
#include "../NodeWrapper.hpp"
#include "../TimeIntervals.hpp"
#include <ImageGraph/internal/Typing.hpp>
#include <QtCore/QDir>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QPushButton>

namespace PhotoGraph::nodes {
namespace __detail {
class ImageButton : public QPushButton {
  Q_OBJECT

  QUrl url_{};

  void updateValue() {
    assert(url_.isLocalFile());
    setText(url_.toLocalFile());
    emit urlChanged(url_);
  }
  void init();

signals:
  void urlChanged(QUrl url);

public:
  QUrl url() const { return url_; }

  QJsonObject toJson() const { return {{"file", url_.toLocalFile()}}; }

  ImageButton(QWidget* parent = nullptr) : QPushButton(parent) { init(); }
  ImageButton(QJsonObject json, QWidget* parent = nullptr)
      : QPushButton(parent), url_{QUrl::fromLocalFile(json.value("file").toString())} {
    init();
    setText(url_.toLocalFile());
  }
};

using ImageInputProxy = DefaultProxy<ImageButton>;
} // namespace __detail

class ImageInputNode : public DefaultImageNode<__detail::ImageInputProxy, ImageInputNode> {
  using in_port_t = NodeImagePort<PortType::INPUT, ImageGraph::OutNode, ImageInputNode>;
  using parent_t = DefaultImageNode<__detail::ImageInputProxy, ImageInputNode>;

  void initializeUrls();
  bool updateUrl(const QUrl& url);
  std::array<ConnectionWrapper, 2> createConnections();

  NodeWrapper<ImageGraph::OutNode> node_;
  std::array<ConnectionWrapper, 2> connections_;
  bool changed_{false};
  time::TimePoints points_;

  auto* outPort() {
    auto& out_ports{this->outPorts()};
    assert(out_ports.size() == 1);
    return out_ports.at(0);
  }

public:
  static inline QString typeID() { return "ImageInputNode"; }

  void inputConnectionWillUpdate(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }
  void inputConnectionUpdated(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }
  void inputConnectionAdded(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }
  void inputConnectionRemoved(in_port_t*) { throw std::invalid_argument("This node has no inputs!"); }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  ImageInputNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{NodeState::INVALID_ARGUMENTS, 0, 1, view, queue, parent}, node_{queue}, connections_{
                                                                                             createConnections()} {
    initializeUrls();
    this->setTitle("Input");
  }
  ImageInputNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                 QGraphicsItem* parent = nullptr)
      : parent_t{json.value("parent").toObject(), registration, view, queue, parent}, node_{queue},
        connections_{createConnections()} {
    initializeUrls();
  }
};
} // namespace PhotoGraph::nodes