#include "ImageGraphView.hpp"
#include "nodes/ImageArithmeticNodes.hpp"
#include "nodes/ImageClampNode.hpp"
#include "nodes/ImageConvertNode.hpp"
#include "nodes/ImageGammaNode.hpp"
#include "nodes/ImageGaussianNode.hpp"
#include "nodes/ImageInputNode.hpp"
#include "nodes/ImageLinearNode.hpp"
#include "nodes/ImageOutputNode.hpp"
#include "nodes/ImageResizeNode.hpp"

using namespace PhotoGraph;
using namespace PhotoGraph::nodes;

ImageGraphView::ImageGraphView(ActionQueue& queue, QGraphicsView* preview, NodeGraphScene* scene, QWidget* parent)
    : NodeGraphView(scene, parent), queue_{queue}, preview_{preview} {
  addTypedNode<ImageInputNode>(QStringLiteral("Input"), QStringList() << QStringLiteral("Input"));

  addTypedNode<ImageOutputNode>(QStringLiteral("Output"), QStringList() << QStringLiteral("Output"));

  addTypedNode<ImageLinearNode>(QStringLiteral("Linear"), QStringList() << QStringLiteral("Per Pixel"));
  addTypedNode<ImageConvertNode>(QStringLiteral("Convert"), QStringList() << QStringLiteral("Per Pixel"));
  addTypedNode<ImageGammaNode>(QStringLiteral("Gamma"), QStringList() << QStringLiteral("Per Pixel"));
  addTypedNode<ImageClampNode>(QStringLiteral("Clamp"), QStringList() << QStringLiteral("Per Pixel"));

  addTypedNode<ImageGaussianBlurNode>(QStringLiteral("Gaußian Blur"), QStringList() << QStringLiteral("Convolution"));

  addTypedNode<ImageResizeNode>(QStringLiteral("Resize"), QStringList());

  addTypedNode<ImageAdditionNode>(QStringLiteral("Addition"), QStringList() << QStringLiteral("Arithmetic"));
  addTypedNode<ImageSubtractionNode>(QStringLiteral("Subtraction"), QStringList() << QStringLiteral("Arithmetic"));
  addTypedNode<ImageMultiplicationNode>(QStringLiteral("Multiplication"), QStringList()
                                                                              << QStringLiteral("Arithmetic"));
  addTypedNode<ImageDivisionNode>(QStringLiteral("Division"), QStringList() << QStringLiteral("Arithmetic"));

  addTypedConnection<ImageConnection>();
}
