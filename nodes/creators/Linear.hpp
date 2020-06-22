#pragma once
#include <ImageGraph/core/nodes/OutNode.hpp>

namespace creators {
struct LinearCreator {
  static std::unique_ptr<ImageGraph::OutNode> createNode(const std::type_info& in_type, const std::type_info& out_type,
                                                         ImageGraph::OutNode& in_node, double constant, double factor,
                                                         bool dither);
};
} // namespace creators
