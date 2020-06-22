#pragma once
#include <ImageGraph/core/nodes/OutNode.hpp>

namespace creators {
struct GaussianBlurCreator {
  static std::unique_ptr<ImageGraph::OutNode> createNode(const std::type_info& in_type, const std::type_info& out_type,
                                                         ImageGraph::OutNode& in_node, double radius,
                                                         double min_amplitude, bool dither);
};
} // namespace creators
