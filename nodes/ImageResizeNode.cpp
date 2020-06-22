#include "ImageResizeNode.hpp"

using namespace PhotoGraph::nodes;
using namespace PhotoGraph::nodes::__detail;

ResizeManager::ResizeManager(ResizeGrid* grid) : OneTypeDitherManager{grid}, grid_{grid} {
  QObject::connect(grid_->xLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    x_ = std::move(value);
    emit parametersUpdated();
  });
  QObject::connect(grid_->yLine(), &DoubleLineEdit::valueChanged, this, [this](std::optional<double> value) {
    y_ = std::move(value);
    emit parametersUpdated();
  });
  QObject::connect(grid_->aLine(), &SizeTypeLineEdit::valueChanged, this, [this](std::optional<std::size_t> value) {
    a_ = std::move(value);
    emit parametersUpdated();
  });
  QObject::connect(grid_->methodCombo(), &ResizeCombo::methodChanged, this, [this](ResizeMethod method) {
    setMethod(std::move(method));
    emit parametersUpdated();
  });
  x_ = grid_->xLine()->value();
  y_ = grid_->yLine()->value();
  a_ = grid_->aLine()->value();
  setMethod(grid_->methodCombo()->currentMethod());
}
