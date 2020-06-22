#include "Clamp.hpp"
#include "../../TypeDefs.hpp"
#include <ImageGraph/core/nodes/impl/PerPixel.hpp>

using namespace creators;
using handler_out_t = std::unique_ptr<ImageGraph::OutNode>;

struct ClampHandler {
  template<typename In, typename Out>
  static inline handler_out_t perform(ImageGraph::OutNode* in, double min, double max, bool dither) {
    auto* out{dynamic_cast<ImageGraph::OutputNode<In>*>(in)};
    assert(out);
    return std::make_unique<ImageGraph::nodes::ClampNode<In, Out>>(*out, dither, min, max);
  }
};

handler_out_t ClampCreator::createNode(const std::type_info& in_type, const std::type_info& out_type,
                                       ImageGraph::OutNode& in_node, double min, double max, bool dither) {
  return TypeListList<default_numbers_t, default_numbers_t>::perform<ClampHandler, handler_out_t>(
             std::tie(in_type, out_type), &in_node, min, max, dither)
      .value();
}
