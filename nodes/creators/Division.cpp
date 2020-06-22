#include "Division.hpp"
#include "../../TypeDefs.hpp"
#include <ImageGraph/core/nodes/impl/PerTwoPixels.hpp>

using namespace creators;
using handler_out_t = std::unique_ptr<ImageGraph::OutNode>;

struct DivisionHandler {
  template<typename T1, typename T2, typename T3>
  static inline handler_out_t perform(ImageGraph::OutNode* in1, ImageGraph::OutNode* in2, bool dither) {
    auto* out1{dynamic_cast<ImageGraph::OutputNode<T1>*>(in1)};
    auto* out2{dynamic_cast<ImageGraph::OutputNode<T2>*>(in2)};
    assert(out1 and out2);
    return std::make_unique<ImageGraph::nodes::DivisionNode<T1, T2, T3>>(*out1, *out2, dither);
  }
};

handler_out_t DivisionCreator::createNode(const TwoDitherInfo& info) {
  return TypeListList<default_numbers_t, default_numbers_t, default_numbers_t>::perform<DivisionHandler, handler_out_t>(
             std::tie(info.in_type1, info.in_type2, info.out_type), &info.in_node1, &info.in_node2, info.dither)
      .value();
}
