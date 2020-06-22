#pragma once
#include <ImageGraph/core/nodes/OutNode.hpp>

namespace creators {
struct TwoDitherInfo {
  const std::type_info &in_type1, &in_type2, &out_type;
  ImageGraph::OutNode &in_node1, &in_node2;
  bool dither;
  TwoDitherInfo(const std::type_info& in_type1, const std::type_info& in_type2, const std::type_info& out_type,
                ImageGraph::OutNode& in_node1, ImageGraph::OutNode& in_node2, bool dither)
      : in_type1{in_type1}, in_type2{in_type2}, out_type{out_type}, in_node1{in_node1}, in_node2{in_node2},
        dither{dither} {}
};
} // namespace creators
