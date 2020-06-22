#include "TestImageGraph.hpp"
#include "ActionQueue.hpp"
#include "ContainerHelp.hpp"
#include <ImageGraph/core/NodeGraph.hpp>
#include <ImageGraph/core/nodes/impl/FileSink.hpp>
#include <ImageGraph/core/nodes/impl/GaussianBlur.hpp>
#include <ImageGraph/core/nodes/impl/PerPixel.hpp>
#include <ImageGraph/core/nodes/impl/Vips.hpp>
#include <ImageGraph/core/optimizers/LUTOptimizer.hpp>

void testImageGraph() {
  std::string folder{"/home/fingolfin/projects/PhotoGraph/img/"};

  using namespace ImageGraph;
  using namespace ImageGraph::optimizers;
  using namespace ImageGraph::nodes;

  NodeGraph graph{};
  graph.createOptimizer<LUTOptimizer<internal::default_numbers_t>>();

  ActionQueue queue{};
  auto* loader{queue.createOutNode<LoadNode<float32_t>>(folder + "PabellonSunset.exr")};
  auto* lineariser{queue.createOutNode<LinearNode<float32_t, float32_t>>(*loader, true, 3.f, 0.f)};
  auto* gammiser{queue.createOutNode<GammaNode<float32_t, float32_t>>(*lineariser, true, .7f)};
  auto* converter{queue.createOutNode<ConvertNode<float32_t, uint8_t>>(*gammiser, true)};
  auto* sinker{queue.createSinkNode<FileSinkNode<uint8_t>>(*converter, folder + "PabellonSunsetTonemapped.png")};

  std::cout << "Before: " << queue << std::endl;
  queue.apply(graph);
  std::cout << "After: " << queue << std::endl;

  // TODO graph.optimize();
  graph.compute(20'000'000);
  std::cout << "computed: " << queue << std::endl;

  queue.removeNode(sinker);
  queue.removeNode(converter);
  std::cout << "removed: " << queue << std::endl;

  auto* convolver1{queue.createOutNode<GaussianBlurNode<float32_t, uint8_t>>(*gammiser, 8.f, .1f, true)};
  std::cout << "added: " << queue << std::endl;

  queue.removeNode(convolver1);
  std::cout << "removed again: " << queue << std::endl;

  auto* convolver2{queue.createOutNode<GaussianBlurNode<float32_t, uint8_t>>(*gammiser, 2.f, 1e-1f, true)};
  sinker = queue.createSinkNode<FileSinkNode<uint8_t>>(*convolver2, folder + "PabellonSunsetBlurred.png");

  std::cout << "Before: " << queue << std::endl;
  queue.apply(graph);
  std::cout << "After: " << queue << std::endl;

  // TODO graph.optimize();
  graph.compute(50'000'000);
}