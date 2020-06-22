#pragma once
#include "ConnectionWrapper.hpp"
#include "DefaultImageNode.hpp"
#include "NodeWrapper.hpp"
#include "TimeIntervals.hpp"
#include <ImageGraph/internal/CompileTime.hpp>

namespace PhotoGraph::nodes {
template<typename Proxy, typename Manager, typename Derived, std::size_t InputNum> class ToOneNode
    : public DefaultImageNode<Proxy, Derived> {
  using in_port_t = NodeImagePort<PortType::INPUT, ImageGraph::OutNode, Derived>;
  using parent_t = DefaultImageNode<Proxy, Derived>;
  using unique_t = std::unique_ptr<ImageGraph::OutNode>;

  Manager param_man_;
  NodeWrapper<ImageGraph::OutNode> node_;
  std::array<ConnectionWrapper, 2> connections_;
  bool changed_{false};
  time::TimePoints points_;

  template<std::size_t Index> auto* inPort() {
    static_assert(Index < InputNum);
    auto& in_ports{this->inPorts()};
    assert(in_ports.size() == InputNum);
    return in_ports.at(Index);
  }
  auto* outPort() {
    auto& out_ports{this->outPorts()};
    assert(out_ports.size() == 1);
    return out_ports.at(0);
  }

  void updateNode(bool prepared) {
    this->prepareGeometryChange();
    if (not param_man_.validInputs()) {
      this->setState(NodeState::INVALID_INPUTS);
      return;
    }
    if (not param_man_.validParams()) {
      this->setState(NodeState::INVALID_ARGUMENTS);
      return;
    }
    auto* out_port{outPort()};
    if (not prepared) out_port->valueWillUpdate();

    node_.reset();
    auto new_node{param_man_.createNode()};
    auto* raw_node{new_node.get()};
    node_.replaceNode(std::move(new_node));
    out_port->updateValue(raw_node);

    this->setState(NodeState::NORMAL);
  }

  std::array<ConnectionWrapper, 2> createConnections() {
    return {ConnectionWrapper{QObject::connect(&param_man_, &Manager::parametersUpdated,
                                               [this] {
                                                 changed_ = true;
                                                 updateNode(false);
                                               })},
            ConnectionWrapper{QObject::connect(this->typedView(), &ImageGraphView::changesApplied, [this] {
              const auto now{time::clock_t::now()};
              if (changed_) {
                points_.insert(now);
                changed_ = false;
              }
              if (node_) node_.get()->setChangeProbability(points_.computeRelevance(now));
            })}};
  }
  template<std::size_t Index = 0> inline bool setInputs(ImageGraph::OutNode* node, in_port_t* port) {
    static_assert(Index < InputNum);
    if constexpr (InputNum == 1) {
      assert(port == inPort<0>());
      param_man_.setInput(node);
      return true;
    } else {
      bool output{false};
      if (inPort<Index>() == port) {
        output = true;
        param_man_.template setInput<Index>(node);
      }
      if constexpr (Index + 1 < InputNum) {
        const bool test{setInputs<Index + 1>(node, port)};
        assert(output != test);
      }
      return output;
    }
  }
  template<std::size_t Index = 0> inline bool orJoin(in_port_t* port) {
    if constexpr (Index == InputNum)
      return false;
    else
      return inPort<Index>() == port or orJoin<Index + 1>(port);
  }
  void assertPort(in_port_t* port) { assert(orJoin(port)); }

public:
  void inputConnectionWillUpdate(in_port_t* port) {
    assertPort(port);
    outPort()->valueWillUpdate();
    setInputs(nullptr, port);
    node_.reset();
    this->setState(NodeState::INVALID_INPUTS);
  }
  void inputConnectionAdded(in_port_t* port) {
    changed_ = true;
    inputConnectionUpdated(port);
  }
  void inputConnectionUpdated(in_port_t* port) {
    assertPort(port);
    setInputs(port->value(), port);
    updateNode(true);
  }
  void inputConnectionRemoved(in_port_t* port) {
    assertPort(port);
    auto* out_port{outPort()};
    out_port->valueWillUpdate();

    setInputs(nullptr, port);
    node_.reset();
    out_port->updateValue(nullptr);

    this->prepareGeometryChange();
    this->setState(NodeState::INVALID_INPUTS);
  }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  ToOneNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{NodeState::INVALID_INPUTS, InputNum, 1, view, queue, parent},
        param_man_{this->innerWidget()->innerWidget()}, node_{queue}, connections_{createConnections()} {
    updateNode(false);
  }
  ToOneNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
            QGraphicsItem* parent = nullptr)
      : parent_t{json.value("parent").toObject(), registration, view, queue, parent},
        param_man_{this->innerWidget()->innerWidget()}, node_{queue}, connections_{createConnections()} {
    updateNode(false);
  }
};
template<typename Proxy, typename Manager, typename Derived> using TwoToOneNode = ToOneNode<Proxy, Manager, Derived, 2>;
template<typename Proxy, typename Manager, typename Derived> using OneToOneNode = ToOneNode<Proxy, Manager, Derived, 1>;
} // namespace PhotoGraph::nodes
