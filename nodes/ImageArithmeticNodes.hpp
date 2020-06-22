#include "../DefaultProxy.hpp"
#include "../JsonWidgets.hpp"
#include "../LabelValueGrid.hpp"
#include "../ParameterManager.hpp"
#include "../ToOneNode.hpp"
#include "../TypeComboBox.hpp"
#include "../TypedLineEdit.hpp"
#include "creators/Addition.hpp"
#include "creators/Division.hpp"
#include "creators/Multiplication.hpp"
#include "creators/Subtraction.hpp"
#include "creators/TwoDitherInfo.hpp"

namespace PhotoGraph::nodes {
namespace __detail {
class ArithmeticGrid : public TwoInTypedList<TypeList<>> {
  using parent_t = TwoInTypedList<TypeList<>>;

public:
  ArithmeticGrid(QWidget* parent = nullptr) : parent_t{std::make_tuple(), parent} {}
  ArithmeticGrid(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

using ArithmeticProxy = DefaultProxy<ArithmeticGrid>;

template<typename Creator> class ArithmeticManager : public TwoTypeDitherManager {
  ArithmeticGrid* grid_{nullptr};
  friend Creator;

  using handler_out_t = std::unique_ptr<ImageGraph::OutNode>;

public:
  bool validParams() const { return outType(); }
  handler_out_t createNode() {
    return Creator::createNode(creators::TwoDitherInfo(*this->inType<0>(), *this->inType<1>(), *this->outType(),
                                                       *this->inNode<0>(), *this->inNode<1>(), this->dither()));
  }

  ArithmeticManager(ArithmeticGrid* grid) : TwoTypeDitherManager{grid}, grid_{grid} {}
};
} // namespace __detail

namespace {
template<typename T> struct Stringer {};
template<> struct Stringer<creators::AdditionCreator> {
  static QString id() { return QStringLiteral("ImageAdditionNode"); }
  static QString title() { return QStringLiteral("Addition"); }
};
template<> struct Stringer<creators::DivisionCreator> {
  static QString id() { return QStringLiteral("ImageDivisionNode"); }
  static QString title() { return QStringLiteral("Division"); }
};
template<> struct Stringer<creators::MultiplicationCreator> {
  static QString id() { return QStringLiteral("ImageMultiplicationNode"); }
  static QString title() { return QStringLiteral("Multiplication"); }
};
template<> struct Stringer<creators::SubtractionCreator> {
  static QString id() { return QStringLiteral("ImageSubtractionNode"); }
  static QString title() { return QStringLiteral("Subtraction"); }
};
} // namespace

template<typename Creator> class ImageArithmeticNode
    : public TwoToOneNode<__detail::ArithmeticProxy, __detail::ArithmeticManager<Creator>,
                          ImageArithmeticNode<Creator>> {
  using parent_t = TwoToOneNode<__detail::ArithmeticProxy, __detail::ArithmeticManager<Creator>, ImageArithmeticNode>;

public:
  static inline QString typeID() { return Stringer<Creator>::id(); }

  ImageArithmeticNode(ImageGraphView* view, ActionQueue& queue, QGraphicsItem* parent = nullptr)
      : parent_t{view, queue, parent} {
    this->setTitle(Stringer<Creator>::title());
  }
  ImageArithmeticNode(QJsonObject json, PortRegistration& registration, ImageGraphView* view, ActionQueue& queue,
                      QGraphicsItem* parent = nullptr)
      : parent_t{json, registration, view, queue, parent} {}
};

using ImageAdditionNode = ImageArithmeticNode<creators::AdditionCreator>;
using ImageSubtractionNode = ImageArithmeticNode<creators::SubtractionCreator>;
using ImageMultiplicationNode = ImageArithmeticNode<creators::MultiplicationCreator>;
using ImageDivisionNode = ImageArithmeticNode<creators::DivisionCreator>;
} // namespace PhotoGraph::nodes
