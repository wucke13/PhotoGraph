#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "../Typing.hpp"
#include "creators/Clamp.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class ClampGrid : public DefaultTypedList<TypeList<DoubleLineEdit, DoubleLineEdit>> {
  Q_OBJECT

  using parent_t = DefaultTypedList<TypeList<DoubleLineEdit, DoubleLineEdit>>;
  using line_t = parent_t::LabeledWidget<DoubleLineEdit>;

public:
  DoubleLineEdit* minLine() const { return this->template get<0>().widget; }
  DoubleLineEdit* maxLine() const { return this->template get<1>().widget; }

  ClampGrid(QWidget* parent = nullptr) : parent_t{std::make_tuple(line_t{"Minimum"}, line_t{"Maximum"}), parent} {}
  ClampGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using ClampProxy = DefaultProxy<ClampGrid>;

class ClampManager : public OneTypeDitherManager {
  Q_OBJECT

  std::optional<double> min_{}, max_{};
  __detail::ClampGrid* grid_{nullptr};

public:
  bool validParams() const {
    if (min_ and max_) {
      assert(outType());
      return true;
    }
    return false;
  }
  std::unique_ptr<ImageGraph::OutNode> createNode() {
    return creators::ClampCreator::createNode(*inType(), *outType(), *inNode(), min_.value(), max_.value(), dither());
  }

  ClampManager(ClampGrid* grid);
};
} // namespace __detail

class ImageClampNode : public OneToOneNode<__detail::ClampProxy, __detail::ClampManager, ImageClampNode> {
  using parent_t = OneToOneNode<__detail::ClampProxy, __detail::ClampManager, ImageClampNode>;

public:
  static inline QString typeID() { return "ImageClampNode"; }

  ImageClampNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle("Clamp");
  }
  ImageClampNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                 QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};
} // namespace PhotoGraph::nodes
