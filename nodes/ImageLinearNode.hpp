#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "../Typing.hpp"
#include "creators/Linear.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class LinearGrid : public DefaultTypedList<TypeList<DoubleLineEdit, DoubleLineEdit>> {
  Q_OBJECT

  using parent_t = DefaultTypedList<TypeList<DoubleLineEdit, DoubleLineEdit>>;
  using line_t = parent_t::LabeledWidget<DoubleLineEdit>;

public:
  DoubleLineEdit* constantLine() const { return this->template get<0>().widget; }
  DoubleLineEdit* factorLine() const { return this->template get<1>().widget; }

  LinearGrid(QWidget* parent = nullptr)
      : parent_t{std::make_tuple(line_t{QStringLiteral("Constant")}, line_t{QStringLiteral("Factor")}), parent} {}
  LinearGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using LinearProxy = DefaultProxy<LinearGrid>;

class LinearManager : public OneTypeDitherManager {
  Q_OBJECT

  std::optional<double> constant_{}, factor_{};
  LinearGrid* grid_{nullptr};

public:
  LinearManager(LinearGrid* grid);

  bool validParams() const {
    if (constant_ and factor_) {
      assert(outType());
      return true;
    }
    return false;
  }
  std::unique_ptr<ImageGraph::OutNode> createNode() {
    return creators::LinearCreator::createNode(*inType(), *outType(), *inNode(), constant_.value(), factor_.value(),
                                               dither());
  }
};
} // namespace __detail

class ImageLinearNode : public OneToOneNode<__detail::LinearProxy, __detail::LinearManager, ImageLinearNode> {
  using parent_t = OneToOneNode<__detail::LinearProxy, __detail::LinearManager, ImageLinearNode>;

public:
  static inline QString typeID() { return QStringLiteral("ImageLinearNode"); }

  ImageLinearNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle(QStringLiteral("Linear"));
  }
  ImageLinearNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                  QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};
} // namespace PhotoGraph::nodes
