#pragma once
#include "ActionQueue.hpp"
#include <memory>
#include <typeinfo>

template<typename Node> class NodeWrapper {
  Node* node_{nullptr};
  ActionQueue& queue_;

public:
  NodeWrapper(ActionQueue& queue) : queue_{queue} {}
  template<typename T> NodeWrapper(ActionQueue& queue, std::unique_ptr<T>&& ptr) : queue_{queue}, node_{ptr.get()} {
    queue_.addNode(std::forward<std::unique_ptr<T>>(ptr));
  }
  ~NodeWrapper() {
    if (node_) queue_.removeNode(node_);
  }

  void reset() {
    if (node_) queue_.removeNode(node_);
    node_ = nullptr;
  }
  Node* get() const { return node_; }
  operator bool() const { return node_; }
  const std::type_info& outputType() const { return node_->outputType(); }
  auto dimensions() const { return node_->dimensions(); }
  template<typename T> void replaceNode(std::unique_ptr<T>&& ptr) {
    if (node_) queue_.removeNode(node_);
    node_ = ptr.get();
    queue_.addNode(std::forward<std::unique_ptr<T>>(ptr));
  }
};
