#include "ImageLinearNode.hpp"

using namespace PhotoGraph::nodes;
using namespace PhotoGraph::nodes::__detail;

LinearManager::LinearManager(LinearGrid* grid) : OneTypeDitherManager{grid}, grid_{grid} {
  QObject::connect(grid_->constantLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    constant_ = std::move(value);
    emit parametersUpdated();
  });
  QObject::connect(grid_->factorLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    factor_ = std::move(value);
    emit parametersUpdated();
  });
  constant_ = grid_->constantLine()->value();
  factor_ = grid_->factorLine()->value();
}
