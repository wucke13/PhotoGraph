#include "ImageGammaNode.hpp"

using namespace PhotoGraph::nodes;
using namespace PhotoGraph::nodes::__detail;

GammaManager::GammaManager(GammaGrid* grid) : OneTypeDitherManager{grid}, grid_{grid} {
  QObject::connect(grid_->gammaLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    gamma_ = std::move(value);
    emit parametersUpdated();
  });
  gamma_ = grid_->gammaLine()->value();
}
