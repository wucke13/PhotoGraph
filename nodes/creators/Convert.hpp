#pragma once
#include <ImageGraph/core/nodes/OutNode.hpp>

namespace creators {
struct ConvertCreator {
  static std::unique_ptr<ImageGraph::OutNode> createNode(const std::type_info& in_type, const std::type_info& out_type,
                                                         ImageGraph::OutNode& in_node, bool dither);
};
} // namespace creators
