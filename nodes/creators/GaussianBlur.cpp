#include "GaussianBlur.hpp"
#include "../../TypeDefs.hpp"
#include <ImageGraph/core/nodes/impl/GaussianBlur.hpp>

using namespace creators;
using handler_out_t = std::unique_ptr<ImageGraph::OutNode>;

struct GaussianBlurHandler {
  template<typename In, typename Out>
  static inline handler_out_t perform(ImageGraph::OutNode* in, double radius, double min_amplitude, bool dither) {
    auto* out{dynamic_cast<ImageGraph::OutputNode<In>*>(in)};
    assert(out);
    return std::make_unique<ImageGraph::nodes::GaussianBlurNode<In, Out>>(*out, radius, min_amplitude, dither);
  }
};

handler_out_t GaussianBlurCreator::createNode(const std::type_info& in_type, const std::type_info& out_type,
                                              ImageGraph::OutNode& in_node, double radius, double min_amplitude,
                                              bool dither) {
  return TypeListList<default_numbers_t, default_numbers_t>::perform<GaussianBlurHandler, handler_out_t>(
             std::tie(in_type, out_type), &in_node, radius, min_amplitude, dither)
      .value();
}
