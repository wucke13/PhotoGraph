#pragma once
#include <ImageGraph/core/nodes/OutNode.hpp>
#include <QNodeGraph/NodeGraphScene.hpp>
#include <QNodeGraph/TypedConnectionItem.hpp>
#include <QNodeGraph/default/DefaultConnectionItem.hpp>
#include <QtCore/QPointF>
#include <sstream>

template<PortType Dir, typename NodeType> class ImagePort;
class ImageConnection final
    : public DefaultConnectionItem<ImagePort<PortType::OUTPUT, ImageGraph::OutNode>,
                                   ImagePort<PortType::INPUT, ImageGraph::OutNode>, ImageConnection> {
  using start_t = ImagePort<PortType::OUTPUT, ImageGraph::OutNode>;
  using end_t = ImagePort<PortType::INPUT, ImageGraph::OutNode>;
  using parent_t = DefaultConnectionItem<start_t, end_t, ImageConnection>;

  ImageConnection(start_t* start, end_t* end, QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t{start, end, temp_end, scene, parent} {}

public:
  static inline QString typeID() { return "ImageConnection"; }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  ImageConnection(start_t* start, QPointF temp_end, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t{start, temp_end, scene, parent} {}
  ImageConnection(QJsonObject json, const PortStorage& ports, NodeGraphScene* scene, QGraphicsItem* parent = nullptr)
      : parent_t{json.value("parent").toObject(), ports, scene, parent} {}
};
