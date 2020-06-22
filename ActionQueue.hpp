#pragma once

#include "ActionMessage.hpp"
#include "PtrHelp.hpp"
#include <ImageGraph/core/Optimizer.hpp>
#include <ImageGraph/core/nodes/OutNode.hpp>
#include <ImageGraph/core/nodes/SinkNode.hpp>
#include <ImageGraph/internal/BorrowedPtr.hpp>
#include <memory>

namespace {
class ProtoMessage final : public Message {
  const ImageGraph::Node* node_;

public:
  const ImageGraph::Node* node() const final { return node_; }
  void apply(ImageGraph::NodeGraph&) final { throw std::runtime_error("apply is not supported on ProtoMessages!"); }

  ProtoMessage(MessageType type, const ImageGraph::Node* node) : Message(type), node_{node} {}
};
} // namespace

class ActionQueue {
  using msg_ptr_t = std::unique_ptr<Message>;
  using values_t = std::list<msg_ptr_t>;
  using lookup_t = std::unordered_map<Message*, values_t::iterator, PtrHash<Message*>, PtrEqual<Message*>>;
  lookup_t lookup_{};
  values_t values_;

  void insert(msg_ptr_t msg) {
    assert(lookup_.find(msg.get()) == lookup_.end());
    auto* ptr{msg.get()};
    auto it{values_.emplace(values_.end(), std::move(msg))};
    lookup_.emplace(ptr, it);
  }
  void erase(lookup_t::iterator it) {
    assert(it != lookup_.end());
    values_.erase(it->second);
    lookup_.erase(it);
  }

public:
  void addNode(std::unique_ptr<ImageGraph::OutNode> node) { insert(std::make_unique<AddOutMessage>(std::move(node))); }
  void addNode(std::unique_ptr<ImageGraph::SinkNode> node) {
    insert(std::make_unique<AddSinkMessage>(std::move(node)));
  }
  template<typename Node, typename... Args> Node* createOutNode(Args&&... args) {
    auto unique_ptr{std::make_unique<Node>(std::forward<Args>(args)...)};
    auto* ptr{unique_ptr.get()};
    addNode(std::move(unique_ptr));
    return ptr;
  }
  template<typename Node, typename... Args> Node* createSinkNode(Args&&... args) {
    auto unique_ptr{std::make_unique<Node>(std::forward<Args>(args)...)};
    auto* ptr{unique_ptr.get()};
    addNode(std::move(unique_ptr));
    return ptr;
  }

  void removeNode(ImageGraph::OutNode* node) {
    auto tmp{std::make_unique<ProtoMessage>(MessageType::ADD_OUT, node)};
    auto it{lookup_.find(tmp.get())};
    if (it != lookup_.end())
      erase(it);
    else
      insert(std::make_unique<RemoveOutMessage>(node));
  }
  void removeNode(ImageGraph::SinkNode* node) {
    auto tmp{std::make_unique<ProtoMessage>(MessageType::ADD_SINK, node)};
    auto it{lookup_.find(tmp.get())};
    if (it != lookup_.end())
      erase(it);
    else
      insert(std::make_unique<RemoveSinkMessage>(node));
  }

  auto begin() const { return values_.begin(); }
  auto end() const { return values_.end(); }

  void apply(ImageGraph::NodeGraph& graph) {
    for (auto& message : values_) message->apply(graph);
    lookup_.clear(), values_.clear();
  }

  ~ActionQueue() {
    lookup_.clear();
    while (not values_.empty()) values_.pop_back();
  }
};
