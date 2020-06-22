#include "ImageGaussianNode.hpp"

using namespace PhotoGraph::nodes;
using namespace PhotoGraph::nodes::__detail;

GaussianBlurManager::GaussianBlurManager(GaussianBlurGrid* grid) : OneTypeDitherManager{grid}, grid_{grid} {
  QObject::connect(grid_->radiusLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    radius_ = std::move(value);
    emit parametersUpdated();
  });
  QObject::connect(grid_->amplitudeLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    min_amplitude_ = std::move(value);
    emit parametersUpdated();
  });
  radius_ = grid_->radiusLine()->value();
  min_amplitude_ = grid_->amplitudeLine()->value();
}
