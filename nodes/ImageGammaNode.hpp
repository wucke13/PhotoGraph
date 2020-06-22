#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "../Typing.hpp"
#include "creators/Gamma.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class GammaGrid : public DefaultTypedList<TypeList<DoubleLineEdit>> {
  Q_OBJECT

  using parent_t = DefaultTypedList<TypeList<DoubleLineEdit>>;
  using line_t = parent_t::LabeledWidget<DoubleLineEdit>;

public:
  DoubleLineEdit* gammaLine() const { return this->template get<0>().widget; }

  GammaGrid(QWidget* parent = nullptr) : parent_t{std::make_tuple(line_t{"Gamma"}), parent} {}
  GammaGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using GammaProxy = DefaultProxy<GammaGrid>;

class GammaManager : public OneTypeDitherManager {
  Q_OBJECT

  std::optional<double> gamma_{};
  GammaGrid* grid_{nullptr};

public:
  bool validParams() const {
    if (gamma_) {
      assert(outType());
      return true;
    }
    return false;
  }
  std::unique_ptr<ImageGraph::OutNode> createNode() {
    return creators::GammaCreator::createNode(*inType(), *outType(), *inNode(), gamma_.value(), dither());
  }

  GammaManager(GammaGrid* grid);
};
} // namespace __detail

class ImageGammaNode : public OneToOneNode<__detail::GammaProxy, __detail::GammaManager, ImageGammaNode> {
  using parent_t = OneToOneNode<__detail::GammaProxy, __detail::GammaManager, ImageGammaNode>;

public:
  static inline QString typeID() { return "ImageGammaNode"; }

  ImageGammaNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle("Gamma");
  }
  ImageGammaNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                 QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};
} // namespace PhotoGraph::nodes
