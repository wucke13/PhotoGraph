#include "Gamma.hpp"
#include "../../TypeDefs.hpp"
#include <ImageGraph/core/nodes/impl/PerPixel.hpp>

using namespace creators;
using handler_out_t = std::unique_ptr<ImageGraph::OutNode>;

struct GammaHandler {
  template<typename In, typename Out>
  static inline handler_out_t perform(ImageGraph::OutNode* in, double gamma, bool dither) {
    auto* out{dynamic_cast<ImageGraph::OutputNode<In>*>(in)};
    assert(out);
    return std::make_unique<ImageGraph::nodes::GammaNode<In, Out>>(*out, dither, gamma);
  }
};

handler_out_t GammaCreator::createNode(const std::type_info& in_type, const std::type_info& out_type,
                                       ImageGraph::OutNode& in_node, double gamma, bool dither) {
  return TypeListList<default_numbers_t, default_numbers_t>::perform<GammaHandler, handler_out_t>(
             std::tie(in_type, out_type), &in_node, gamma, dither)
      .value();
}
