#pragma once
#include "TwoDitherInfo.hpp"

namespace creators {
struct DivisionCreator {
  static std::unique_ptr<ImageGraph::OutNode> createNode(const TwoDitherInfo&);
};
} // namespace creators
