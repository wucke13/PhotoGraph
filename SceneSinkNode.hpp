#pragma once
#include "MagicTricks.hpp"
#include <ImageGraph/core/Rectangle.hpp>
#include <ImageGraph/core/nodes/impl/SimpleSink.hpp>

class SceneSinkNode final : public ImageGraph::nodes::SimpleSinkNode<uint8_t> {
  MagicGraphicsScene* scene_;
  MagicObject magic_{};
  double relevance_{0};

protected:
  void handleTile(std::shared_ptr<ImageGraph::Tile<uint8_t>> tile) const final { emit magic_.tileSignal(tile); }

public:
  void setRelevance(double relevance) { relevance_ = relevance; }
  relevance_t relevance() const final { return relevance_; }

  SceneSinkNode(ImageGraph::OutputNode<uint8_t>& input, MagicGraphicsScene* scene)
      : ImageGraph::nodes::SimpleSinkNode<uint8_t>{input}, scene_{scene} {
    QObject::connect(&magic_, &MagicObject::tileSignal, scene_, &MagicGraphicsScene::addTile);
  }
};
