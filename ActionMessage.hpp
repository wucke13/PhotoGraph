#pragma once

#include <ImageGraph/core/NodeGraph.hpp>
#include <ImageGraph/core/nodes/Node.hpp>
#include <ImageGraph/internal/Hashing.hpp>
#include <memory>
#include <ostream>
#include <stdexcept>
#include <type_traits>

enum class MessageType { ADD_OUT, ADD_SINK, REMOVE_OUT, REMOVE_SINK };
static inline std::ostream& operator<<(std::ostream& stream, MessageType type) {
  switch (type) {
    case MessageType::ADD_OUT: return stream << "ADD_OUT";
    case MessageType::ADD_SINK: return stream << "ADD_SINK";
    case MessageType::REMOVE_OUT: return stream << "REMOVE_OUT";
    case MessageType::REMOVE_SINK: return stream << "REMOVE_SINK";
  }
  throw std::runtime_error("The MessageType does not work???");
}

class Message {
  const MessageType type_;

public:
  MessageType type() const { return type_; }
  virtual const ImageGraph::Node* node() const = 0;

  virtual void apply(ImageGraph::NodeGraph&) = 0;
  bool operator==(const Message& other) const { return this->type() == other.type() and this->node() == other.node(); }
  friend std::ostream& operator<<(std::ostream& stream, const Message& message) {
    stream << "[" << message.type_ << ": ";
    auto* node{message.node()};
    if (node)
      stream << *node;
    else
      stream << "nullptr";
    return stream << "]";
  }

  Message(MessageType type) : type_{type} {}
  virtual ~Message() = default;
};

namespace std {
template<> struct hash<Message> {
  std::size_t operator()(const Message& m) const noexcept {
    using message_t = std::underlying_type_t<MessageType>;
    return ImageGraph::internal::hash_combine(std::hash<const ImageGraph::Node*>{}(m.node()),
                                              std::hash<message_t>{}(static_cast<message_t>(m.type())));
  }
};
} // namespace std

class AddOutMessage final : public Message {
  std::unique_ptr<ImageGraph::OutNode> node_;

public:
  AddOutMessage(std::unique_ptr<ImageGraph::OutNode> node) : Message{MessageType::ADD_OUT}, node_{std::move(node)} {}
  const ImageGraph::Node* node() const final { return node_.get(); }
  void apply(ImageGraph::NodeGraph& graph) final { graph.addOutNode(std::move(node_)); }
};

class AddSinkMessage final : public Message {
  std::unique_ptr<ImageGraph::SinkNode> node_;

public:
  AddSinkMessage(std::unique_ptr<ImageGraph::SinkNode> node) : Message{MessageType::ADD_SINK}, node_{std::move(node)} {}
  const ImageGraph::Node* node() const final { return node_.get(); }
  void apply(ImageGraph::NodeGraph& graph) final { graph.addSinkNode(std::move(node_)); }
};

class RemoveOutMessage : public Message {
  ImageGraph::OutNode* node_;

public:
  RemoveOutMessage(ImageGraph::OutNode* node) : Message{MessageType::REMOVE_OUT}, node_{node} {}
  const ImageGraph::Node* node() const final { return node_; }
  void apply(ImageGraph::NodeGraph& graph) final { graph.eraseOutNode(*node_); }
};

class RemoveSinkMessage : public Message {
  ImageGraph::SinkNode* node_;

public:
  RemoveSinkMessage(ImageGraph::SinkNode* node) : Message{MessageType::REMOVE_SINK}, node_{node} {}
  const ImageGraph::Node* node() const final { return node_; }
  void apply(ImageGraph::NodeGraph& graph) final { graph.eraseSinkNode(*node_); }
};
