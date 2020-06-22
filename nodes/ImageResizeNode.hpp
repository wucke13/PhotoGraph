#pragma once
#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "../Typing.hpp"
#include "creators/Resize.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class ResizeCombo : public QComboBox {
  Q_OBJECT

  static ResizeMethod indexToMethod(int index) {
    switch (index) {
      case 0: return ResizeMethod::NEAREST;
      case 1: return ResizeMethod::BLOCK;
      case 2: return ResizeMethod::BILINEAR;
      case 3: return ResizeMethod::BICUBIC;
      case 4: return ResizeMethod::LANCZOS;
      default: throw std::invalid_argument("The given index is not supported!");
    }
  }

  void init() {
    addItem(QStringLiteral("Nearest Neighbour"));
    addItem(QStringLiteral("Block"));
    addItem(QStringLiteral("Bilinear"));
    addItem(QStringLiteral("Bicubic"));
    addItem(QStringLiteral("Lanczos"));
    QObject::connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
                     [this](int index) { emit methodChanged(indexToMethod(index)); });
  }

signals:
  void methodChanged(ResizeMethod);

public:
  QJsonObject toJson() const { return {{"current", this->currentIndex()}}; }

  ResizeMethod currentMethod() const { return indexToMethod(currentIndex()); }

  ResizeCombo(QWidget* parent = nullptr) : QComboBox{parent} { init(); }
  ResizeCombo(QJsonObject json, QWidget* parent = nullptr) : QComboBox{parent} {
    init();
    this->setCurrentIndex(json.value("current").toInt());
  }
};

class ResizeGrid : public DefaultTypedList<TypeList<ResizeCombo, DoubleLineEdit, DoubleLineEdit, SizeTypeLineEdit>> {
  Q_OBJECT

  using parent_t = DefaultTypedList<TypeList<ResizeCombo, DoubleLineEdit, DoubleLineEdit, SizeTypeLineEdit>>;
  using combo_t = parent_t::LabeledWidget<ResizeCombo>;
  using line_t = parent_t::LabeledWidget<DoubleLineEdit>;
  using size_line_t = parent_t::LabeledWidget<SizeTypeLineEdit>;

public:
  ResizeCombo* methodCombo() const { return this->template get<0>().widget; }
  DoubleLineEdit* xLine() const { return this->template get<1>().widget; }
  DoubleLineEdit* yLine() const { return this->template get<2>().widget; }
  SizeTypeLineEdit* aLine() const { return this->template get<3>().widget; }
  void setALineVisible(bool visible) {
    auto& pair{this->template get<3>()};
    pair.label->setVisible(visible), pair.widget->setVisible(visible);
  }

  ResizeGrid(QWidget* parent = nullptr)
      : parent_t{std::make_tuple(combo_t{"Method"}, line_t{"x Scale"}, line_t{"y Scale"}, size_line_t{"a Parameter"}),
                 parent} {}
  ResizeGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using ResizeProxy = DefaultProxy<ResizeGrid>;

class ResizeManager : public OneTypeDitherManager {
  Q_OBJECT

  ResizeMethod method_;
  std::optional<double> x_{}, y_{}, a_{};
  __detail::ResizeGrid* grid_{nullptr};

  void setMethod(ResizeMethod method) {
    grid_->setALineVisible(method == ResizeMethod::LANCZOS);
    method_ = std::move(method);
  }

public:
  ResizeManager(ResizeGrid* grid);

  bool validParams() const {
    if (x_ and y_ and (method_ != ResizeMethod::LANCZOS or a_)) {
      assert(outType());
      return true;
    }
    return false;
  }
  std::unique_ptr<ImageGraph::OutNode> createNode() {
    return creators::ResizeCreator::createNode(*inType(), *outType(), *inNode(), method_, x_.value(), y_.value(), a_,
                                               dither());
  }
};
} // namespace __detail

class ImageResizeNode : public OneToOneNode<__detail::ResizeProxy, __detail::ResizeManager, ImageResizeNode> {
  using parent_t = OneToOneNode<__detail::ResizeProxy, __detail::ResizeManager, ImageResizeNode>;

public:
  static inline QString typeID() { return "ImageResizeNode"; }

  ImageResizeNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle("Resize");
  }
  ImageResizeNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                  QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};
} // namespace PhotoGraph::nodes
