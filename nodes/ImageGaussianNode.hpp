#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "../Typing.hpp"
#include "creators/GaussianBlur.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class GaussianBlurGrid : public DefaultTypedList<TypeList<DoubleLineEdit, DoubleLineEdit>> {
  Q_OBJECT

  using parent_t = DefaultTypedList<TypeList<DoubleLineEdit, DoubleLineEdit>>;
  using line_t = parent_t::LabeledWidget<DoubleLineEdit>;

public:
  DoubleLineEdit* radiusLine() const { return this->template get<0>().widget; }
  DoubleLineEdit* amplitudeLine() const { return this->template get<1>().widget; }

  GaussianBlurGrid(QWidget* parent = nullptr)
      : parent_t{std::make_tuple(line_t{"Radius"}, line_t{"Minimum Amplitude"}), parent} {}
  GaussianBlurGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using GaussianBlurProxy = DefaultProxy<GaussianBlurGrid>;

class GaussianBlurManager : public OneTypeDitherManager {
  Q_OBJECT

  std::optional<double> radius_{}, min_amplitude_{};
  GaussianBlurGrid* grid_{nullptr};

public:
  bool validParams() const {
    if (radius_ and min_amplitude_) {
      assert(outType());
      return true;
    }
    return false;
  }
  std::unique_ptr<ImageGraph::OutNode> createNode() {
    return creators::GaussianBlurCreator::createNode(*inType(), *outType(), *inNode(), radius_.value(),
                                                     min_amplitude_.value(), dither());
  }

  GaussianBlurManager(GaussianBlurGrid* grid);
};
} // namespace __detail

class ImageGaussianBlurNode
    : public OneToOneNode<__detail::GaussianBlurProxy, __detail::GaussianBlurManager, ImageGaussianBlurNode> {
  using parent_t = OneToOneNode<__detail::GaussianBlurProxy, __detail::GaussianBlurManager, ImageGaussianBlurNode>;

public:
  static inline QString typeID() { return "ImageGaussianBlurNode"; }

  ImageGaussianBlurNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle("Gaußian Blur");
  }
  ImageGaussianBlurNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                        QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};
} // namespace PhotoGraph::nodes
