#include "ImageClampNode.hpp"

using namespace PhotoGraph::nodes;
using namespace PhotoGraph::nodes::__detail;

ClampManager::ClampManager(ClampGrid* grid) : OneTypeDitherManager{grid}, grid_{grid} {
  QObject::connect(grid_->minLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    min_ = std::move(value);
    emit parametersUpdated();
  });
  QObject::connect(grid_->maxLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    max_ = std::move(value);
    emit parametersUpdated();
  });
  min_ = grid_->minLine()->value();
  max_ = grid_->maxLine()->value();
}
