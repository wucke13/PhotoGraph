#pragma once
#include "JsonWidgets.hpp"
#include "TypeComboBox.hpp"
#include "Typing.hpp"
#include <ImageGraph/core/nodes/OutNode.hpp>
#include <QtCore/QObject>

class TypedDitherManager : public QObject {
  Q_OBJECT

  bool dither_{};
  const std::type_info* out_type_{nullptr};

protected:
  bool dither() const { return dither_; }
  const std::type_info* outType() const { return out_type_; }
  void setOutType(const std::type_info* info) { out_type_ = info; }

signals:
  void parametersUpdated();

public:
  template<typename Widget> TypedDitherManager(Widget* widget) {
    QObject::connect(widget->ditherBox(), &QCheckBox::stateChanged, this, [this](int value) {
      assert(value == Qt::Checked or value == Qt::Unchecked);
      dither_ = value == Qt::Checked;
      emit parametersUpdated();
    });
    const int check_state{widget->ditherBox()->checkState()};
    assert(check_state == Qt::Checked or check_state == Qt::Unchecked);
    dither_ = check_state == Qt::Checked;
  }
};

class OneTypeDitherManager : public TypedDitherManager {
  Q_OBJECT

  ImageGraph::OutNode* input_node_{nullptr};
  DefaultOneInTypeCombo* combo_;

protected:
  void setInputType(const std::type_info* type) { combo_->updateInputType(type); }
  ImageGraph::OutNode* inNode() const { return input_node_; }
  const std::type_info* inType() const { return input_node_ ? &input_node_->outputType() : nullptr; }

public:
  void setInput(ImageGraph::OutNode* node) {
    input_node_ = node;
    this->setInputType(node ? &node->outputType() : nullptr);
  }
  bool validInputs() const { return input_node_; }

  template<typename Widget> OneTypeDitherManager(Widget* widget)
      : TypedDitherManager{widget}, combo_{widget->typeCombo()} {
    QObject::connect(combo_, &TypeComboBox::selectedTypeChanged, this, [this](type_info info, bool index_changed) {
      setOutType(info.info);
      if (index_changed) emit parametersUpdated();
    });
    this->setOutType(combo_->currentType());
  }
};

class TwoTypeDitherManager : public TypedDitherManager {
  Q_OBJECT

  ImageGraph::OutNode *in1_{nullptr}, *in2_{nullptr};
  DefaultTwoInTypeCombo* combo_;

protected:
  template<std::size_t Index> void setInputType(const std::type_info* type) {
    combo_->template updateInputType<Index>(type);
  }
  template<std::size_t Index> ImageGraph::OutNode* inNode() const {
    static_assert(Index < 2);
    if constexpr (Index == 0)
      return in1_;
    else
      return in2_;
  }
  template<std::size_t Index> const std::type_info* inType() const {
    static_assert(Index < 2);
    if constexpr (Index == 0)
      return in1_ ? &in1_->outputType() : nullptr;
    else
      return in2_ ? &in2_->outputType() : nullptr;
  }

public:
  template<std::size_t Index> void setInput(ImageGraph::OutNode* node) {
    static_assert(Index < 2);
    if constexpr (Index == 0)
      in1_ = node;
    else
      in2_ = node;
    this->template setInputType<Index>(node ? &node->outputType() : nullptr);
  }
  bool validInputs() const { return in1_ and in2_; }

  template<typename Widget> TwoTypeDitherManager(Widget* widget)
      : TypedDitherManager{widget}, combo_{widget->typeCombo()} {
    QObject::connect(combo_, &TypeComboBox::selectedTypeChanged, this, [this](type_info info, bool index_changed) {
      setOutType(info.info);
      if (index_changed) emit parametersUpdated();
    });
  }
};
