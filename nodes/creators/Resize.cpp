#include "Resize.hpp"
#include "../../TypeDefs.hpp"
#include <ImageGraph/core/nodes/impl/Resize.hpp>

using namespace creators;
using handler_out_t = std::unique_ptr<ImageGraph::OutNode>;

struct LanczosResizeTypeHandler {
  template<typename T1, typename T2>
  static inline handler_out_t perform(ImageGraph::OutNode* input, double x, double y, std::size_t a, bool dither) {
    auto* out_input{dynamic_cast<ImageGraph::OutputNode<T1>*>(input)};
    assert(out_input);
    return std::make_unique<ImageGraph::nodes::LanczosResizeNode<T1, T2>>(*out_input, x, y, dither, a);
  }
};
template<template<typename, typename> typename Node> struct OtherResizeTypeHandler {
  template<typename T1, typename T2>
  static inline handler_out_t perform(ImageGraph::OutNode* input, double x, double y, bool dither) {
    auto* out_input{dynamic_cast<ImageGraph::OutputNode<T1>*>(input)};
    assert(out_input);
    return std::make_unique<Node<T1, T2>>(*out_input, x, y, dither);
  }
};

handler_out_t ResizeCreator::createNode(const std::type_info& in_type, const std::type_info& out_type,
                                        ImageGraph::OutNode& in_node, ResizeMethod method, double x, double y,
                                        std::optional<std::size_t> a, bool dither) {
  switch (method) {
    case ResizeMethod::NEAREST:
      return TypeListList<default_numbers_t, default_numbers_t>::perform<
                 OtherResizeTypeHandler<ImageGraph::nodes::NearestNeighbourResizeNode>, handler_out_t>(
                 std::tie(in_type, out_type), &in_node, x, y, dither)
          .value();
    case ResizeMethod::BLOCK:
      return TypeListList<default_numbers_t, default_numbers_t>::perform<
                 OtherResizeTypeHandler<ImageGraph::nodes::BlockResizeNode>, handler_out_t>(std::tie(in_type, out_type),
                                                                                            &in_node, x, y, dither)
          .value();
    case ResizeMethod::BILINEAR:
      return TypeListList<default_numbers_t, default_numbers_t>::perform<
                 OtherResizeTypeHandler<ImageGraph::nodes::BilinearResizeNode>, handler_out_t>(
                 std::tie(in_type, out_type), &in_node, x, y, dither)
          .value();
    case ResizeMethod::BICUBIC:
      return TypeListList<default_numbers_t, default_numbers_t>::perform<
                 OtherResizeTypeHandler<ImageGraph::nodes::BicubicResizeNode>, handler_out_t>(
                 std::tie(in_type, out_type), &in_node, x, y, dither)
          .value();
    case ResizeMethod::LANCZOS:
      return TypeListList<default_numbers_t, default_numbers_t>::perform<LanczosResizeTypeHandler, handler_out_t>(
                 std::tie(in_type, out_type), &in_node, x, y, a.value(), dither)
          .value();
  }
  throw std::runtime_error("The method is not valid!");
}
