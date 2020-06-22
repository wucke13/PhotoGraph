#include "ImageOutputNode.hpp"
#include <ImageGraph/core/nodes/impl/PerPixel.hpp>
#include <iomanip>

using namespace PhotoGraph::nodes;

struct HandlerInfo {
  std::unique_ptr<ImageGraph::OutNode> converter;
  std::unique_ptr<SceneSinkNode> node;
  HandlerInfo(std::unique_ptr<ImageGraph::OutNode> converter, std::unique_ptr<SceneSinkNode> node)
      : converter{std::move(converter)}, node{std::move(node)} {}
};
struct InputTypeHandler {
  template<typename T> static inline HandlerInfo perform(ImageGraph::OutNode* input, MagicGraphicsScene* scene) {
    {
      const std::size_t channels{input->channels()};
      if (channels != 1 and channels != 3 and channels != 4) return {nullptr, nullptr};
    }
    auto* out_input{dynamic_cast<ImageGraph::OutputNode<T>*>(input)};
    assert(out_input);
    if constexpr (std::is_same_v<T, uint8_t>)
      return {nullptr, std::make_unique<SceneSinkNode>(*out_input, scene)};
    else {
      auto converter{std::make_unique<ImageGraph::nodes::ConvertNode<T, uint8_t>>(*out_input, true)};
      auto& converter_ref{*converter};
      return {std::move(converter), std::make_unique<SceneSinkNode>(converter_ref, scene)};
    }
  }
};

void ImageOutputNode::initConnections() {
  QObject::connect(this->innerWidget()->innerWidget(), &QPushButton::clicked, this, [this](bool) { showScene(); });
  QObject::connect(this->typedView(), &ImageGraphView::changesApplied, this, [this] { handleApplication(); });
  QObject::connect(this->typedView(), &ImageGraphView::computationStarts, this, [this] { handleComputation(); });
  QObject::connect(this->typedView(), &ImageGraphView::previewSceneChanges, this,
                   [this](QGraphicsScene* old_scene, QGraphicsScene* new_scene) {
                     if (old_scene == new_scene) return;
                     time_point_t current{time::clock_t::now()};
                     use_intervals_.cleanup(current);
                     if (old_scene == &scene_) use_intervals_.endCurrent(current);
                     if (new_scene == &scene_) use_intervals_.setCurrentStart(current);
                     std::cout << "previewSceneChanges: " << old_scene << " -> " << new_scene << ": ";
                     std::cout << std::setprecision(5);
                     std::cout << "value: " << use_intervals_.computeRelevance(current, creation_) << std::endl;
                   });
}

void ImageOutputNode::showPreviewScene(MagicGraphicsScene* scene) { this->typedView()->setPreviewScene(scene); }

void ImageOutputNode::handleApplication() {
  assert(0 <= epsilon_ and epsilon_ < 1);
  if (node_)
    node_.get()->setRelevance(epsilon_ +
                              (1. - epsilon_) * use_intervals_.computeRelevance(time::clock_t::now(), creation_));
}
void ImageOutputNode::handleComputation() {
  scene_.clear();
  if (node_) {
    const auto dimensions{node_.dimensions()};
    scene_.setSceneRect(0, 0, qreal(dimensions.width()), qreal(dimensions.height()));
  }
}

void ImageOutputNode::inputConnectionWillUpdate(in_port_t* port) {
  assert(port == this->inPorts().at(0) and this->inPorts().size() == 1);
  node_.reset();
  converter_.reset();
  setState(NodeState::INVALID_INPUTS);
}
void ImageOutputNode::inputConnectionUpdated(in_port_t* port) {
  assert(port == this->inPorts().at(0) and this->inPorts().size() == 1);
  prepareGeometryChange();
  auto* value{port->value()};
  if (value) {
    node_.reset();
    converter_.reset();
    HandlerInfo info{ImageGraph::internal::default_numbers_t::perform<InputTypeHandler, HandlerInfo>(
                         value->outputType(), value, &scene_)
                         .value()};
    assert(not info.converter or info.node);
    if (not info.node) {
      setState(NodeState::INVALID_INPUTS, "The input has an invalid number of channels!");
      return;
    }
    node_.replaceNode(std::move(info.node));
    if (info.converter) converter_.replaceNode(std::move(info.converter));
  }
  this->setState(value ? NodeState::NORMAL : NodeState::INVALID_INPUTS);
}

void ImageOutputNode::inputConnectionRemoved(in_port_t* port) {
  assert(port->node() == this);
  node_.reset();
  converter_.reset();
  this->setState(NodeState::MISSING_INPUTS);
}
