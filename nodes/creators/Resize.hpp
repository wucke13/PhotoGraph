#pragma once
#include "../../ResizeMethod.hpp"
#include <ImageGraph/core/nodes/OutNode.hpp>

namespace creators {
struct ResizeCreator {
  static std::unique_ptr<ImageGraph::OutNode> createNode(const std::type_info& in_type, const std::type_info& out_type,
                                                         ImageGraph::OutNode& in_node, ResizeMethod method, double x,
                                                         double y, std::optional<std::size_t> a, bool dither);
};
} // namespace creators
