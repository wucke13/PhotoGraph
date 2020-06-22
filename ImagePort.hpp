#pragma once
#include "ImageConnection.hpp"
#include <QNodeGraph/TypedPortItem.hpp>
#include <QNodeGraph/default/DefaultPortItem.hpp>
#include <QNodeGraph/default/UniformConnectionContainer.hpp>

namespace __detail {
template<PortType Dir, typename NodeType> class __ImagePort
    : public DefaultPortItem<Dir, UniformConnectionContainer<ImageConnection>, ImageConnection> {
public:
  using connection_t = ImageConnection;
  using data_t = UniformConnectionContainer<connection_t>;
  using parent_t = DefaultPortItem<Dir, data_t, ImageConnection>;

private:
  NodeType* value_{};

protected:
  void setValue(NodeType* new_value) { value_ = new_value; }

  __ImagePort(data_t data, QGraphicsItem* parent = nullptr) : parent_t{std::move(data), parent} {}

public:
  NodeType* value() const { return value_; }

  QJsonObject toJson() const override { return {{"parent", parent_t::toJson()}}; }

  __ImagePort(QGraphicsItem* parent = nullptr) : parent_t{parent} {}
  __ImagePort(QJsonObject json, QGraphicsItem* parent = nullptr) : parent_t{json, parent} {}
};
} // namespace __detail

template<PortType Dir, typename NodeType> class ImagePort {};

template<typename NodeType> class ImagePort<PortType::INPUT, NodeType>
    : public __detail::__ImagePort<PortType::INPUT, NodeType> {
  using parent_t = __detail::__ImagePort<PortType::INPUT, NodeType>;

protected:
  ImagePort(typename parent_t::data_t data, QGraphicsItem* parent = nullptr) : parent_t{std::move(data), parent} {}

public:
  virtual void preparePropaganda() = 0;
  virtual void acceptPropaganda(NodeType*) = 0;

  ImagePort(QGraphicsItem* parent = nullptr) : parent_t{parent} {}
};
template<typename NodeType> class ImagePort<PortType::OUTPUT, NodeType>
    : public __detail::__ImagePort<PortType::OUTPUT, NodeType> {
  using parent_t = __detail::__ImagePort<PortType::OUTPUT, NodeType>;

protected:
  ConnectionItem* createConnection(QGraphicsSceneMouseEvent* event) override {
    auto* connection{new ImageConnection(this, event->scenePos(), this->view()->nodeGraphScene())};
    this->addConnection(connection);
    return connection;
  }

public:
  void valueWillUpdate() { this->preparePropagation(); }
  void updateValue(NodeType* new_value) {
    if (new_value)
      std::cout << *new_value << std::endl;
    else
      std::cout << "nullptr" << std::endl;
    this->setValue(new_value);
    this->preparePropagation();
    this->propagate();
  }

  template<typename... Args> ImagePort(Args&&... args) : parent_t{std::forward<Args>(args)...} {}
  ImagePort(QJsonObject json, QGraphicsItem* parent = nullptr) : parent_t{json, parent} {}
};

namespace __detail {
template<PortType Dir, typename NodeType, typename Node, typename Derived> class __NodeImagePort
    : public ImagePort<Dir, NodeType> {
  Node* node_;

protected:
  using parent_t = ImagePort<Dir, NodeType>;

  Node* typedNode() const { return node_; }
  void connectionAdded(ImageConnection* connection) override {
    if constexpr (Dir == PortType::INPUT) {
      this->setValue(connection->typedStartItem()->value());
      node_->inputConnectionAdded(static_cast<Derived*>(this));
    }
  }
  void connectionRemoved(ImageConnection*) override {
    if constexpr (Dir == PortType::INPUT) {
      this->setValue(nullptr);
      node_->inputConnectionRemoved(static_cast<Derived*>(this));
    }
  }

public:
  NodeItemGroup* node() const final { return typedNode(); }
  QPointF localCentre() const final { return node_->template portCentre<Dir>(static_cast<const Derived*>(this)); }
  qreal radius() const final { return node_->template portRadius<Dir>(static_cast<const Derived*>(this)); }

  template<typename... Args> __NodeImagePort(Node* node, Args&&... args)
      : parent_t{std::forward<Args>(args)...}, node_{node} {}
  __NodeImagePort(QJsonObject json, Node* node, QGraphicsItem* parent = nullptr)
      : __NodeImagePort{node, json, parent} {}
};
} // namespace __detail

template<PortType Dir, typename NodeType, typename Node> class NodeImagePort final
    : public __detail::__NodeImagePort<Dir, NodeType, Node, NodeImagePort<Dir, NodeType, Node>> {
protected:
  using parent_t = __detail::__NodeImagePort<Dir, NodeType, Node, NodeImagePort<Dir, NodeType, Node>>;

public:
  template<typename... Args> NodeImagePort(Args&&... args) : parent_t{std::forward<Args>(args)...} {}
  NodeImagePort(QJsonObject json, QGraphicsItem* parent = nullptr) : parent_t{json, parent} {}
};
template<typename NodeType, typename Node> class NodeImagePort<PortType::INPUT, NodeType, Node>
    : public __detail::__NodeImagePort<PortType::INPUT, NodeType, Node,
                                       NodeImagePort<PortType::INPUT, NodeType, Node>> {
protected:
  using parent_t =
      __detail::__NodeImagePort<PortType::INPUT, NodeType, Node, NodeImagePort<PortType::INPUT, NodeType, Node>>;

public:
  void preparePropaganda() override {
    this->setValue(nullptr);
    this->typedNode()->inputConnectionWillUpdate(this);
  }
  void acceptPropaganda(NodeType* value) override {
    this->setValue(value);
    this->typedNode()->inputConnectionUpdated(this);
  }

  template<typename... Args> NodeImagePort(Args&&... args) : parent_t{std::forward<Args>(args)...} {}
  NodeImagePort(QJsonObject json, Node* node, QGraphicsItem* parent = nullptr) : parent_t{json, node, parent} {}
};
