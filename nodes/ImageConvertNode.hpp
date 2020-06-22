#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "creators/Convert.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class ConvertGrid : public DefaultTypedList<TypeList<>> {
  Q_OBJECT

  using parent_t = DefaultTypedList<TypeList<>>;

public:
  ConvertGrid(QWidget* parent = nullptr) : parent_t{std::make_tuple(), parent} {}
  ConvertGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using ConvertProxy = DefaultProxy<ConvertGrid>;

class ConvertManager : public OneTypeDitherManager {
  Q_OBJECT

  ConvertGrid* grid_{nullptr};

public:
  bool validParams() const { return outType(); }
  std::unique_ptr<ImageGraph::OutNode> createNode() {
    return creators::ConvertCreator::createNode(*inType(), *outType(), *inNode(), dither());
  }

  ConvertManager(ConvertGrid* grid) : OneTypeDitherManager{grid}, grid_{grid} {}
};
} // namespace __detail

class ImageConvertNode : public OneToOneNode<__detail::ConvertProxy, __detail::ConvertManager, ImageConvertNode> {
  using parent_t = OneToOneNode<__detail::ConvertProxy, __detail::ConvertManager, ImageConvertNode>;

public:
  static inline QString typeID() { return "ImageConvertNode"; }

  ImageConvertNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle("Convert");
  }
  ImageConvertNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                   QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};
} // namespace PhotoGraph::nodes
