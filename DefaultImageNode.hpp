#pragma once

#include "ImageGraphView.hpp"
#include "ImagePort.hpp"
#include <ImageGraph/core/nodes/OutNode.hpp>
#include <QNodeGraph/TypedNodeItemGroup.hpp>
#include <QNodeGraph/default/DefaultBackgroundItem.hpp>
#include <QNodeGraph/default/UniformPortContainer.hpp>

template<typename Proxy, typename Derived> class DefaultImageNode
    : public TypedNodeItemGroup<
          Proxy, DefaultBackgroundItem<Derived>,
          UniformPortContainer<Derived, NodeImagePort<PortType::INPUT, ImageGraph::OutNode, Derived>>,
          UniformPortContainer<Derived, NodeImagePort<PortType::OUTPUT, ImageGraph::OutNode, Derived>>, ImageGraphView,
          Derived> {
  using in_port_t = NodeImagePort<PortType::INPUT, ImageGraph::OutNode, Derived>;
  using out_port_t = NodeImagePort<PortType::OUTPUT, ImageGraph::OutNode, Derived>;
  using parent_t = TypedNodeItemGroup<Proxy, DefaultBackgroundItem<Derived>, UniformPortContainer<Derived, in_port_t>,
                                      UniformPortContainer<Derived, out_port_t>, ImageGraphView, Derived>;

protected:
  ActionQueue& queue_;

public:
  DefaultImageNode(NodeState state, std::size_t initial_inputs, std::size_t initial_outputs, ImageGraphView* view,
                   ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{state, initial_inputs, initial_outputs, view, parent}, queue_{queue} {}
  DefaultImageNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                   QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, parent}, queue_{queue} {}
};
