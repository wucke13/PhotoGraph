#pragma once
#include "Typing.hpp"
#include <QtCore/QJsonObject>
#include <QtWidgets/QComboBox>
#include <typeinfo>

class TypeComboBox : public QComboBox {
  Q_OBJECT

protected:
  struct StringType {
    QString name;
    const std::type_info* type_info;
    StringType(QString name, const std::type_info* type_info) : name{std::move(name)}, type_info{type_info} {}
  };

  void update(std::size_t index, const QString& name, const std::type_info* type_info) {
    auto& string_type{string_types_.at(index)};
    string_type.name = name;
    string_type.type_info = type_info;
    const auto int_index{int(index)};
    setItemText(int_index, name);
    if (int_index == currentIndex()) emit selectedTypeChanged(type_info, false);
  }

private:
  std::vector<StringType> string_types_;

signals:
  void selectedTypeChanged(type_info, bool index_changed);

public:
  /**
   * @brief CAUTION: This function does not store the item texts,
   * which an implementation should either know without storing it or store itself!
   * @return QJsonObject The information to be stored.
   */
  QJsonObject toJson() const { return {{"current", this->currentIndex()}}; }
  const std::type_info* currentType() { return string_types_.at(size_t(this->currentIndex())).type_info; }

  TypeComboBox(std::vector<StringType> string_types) : string_types_{std::move(string_types)} {
    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
      type_info info{string_types_.at(size_t(index)).type_info};
      emit selectedTypeChanged(info, true);
    });
    for (const StringType& string_type : string_types_) addItem(string_type.name);
  }
  TypeComboBox(QJsonObject json, std::vector<StringType> string_types) : TypeComboBox{std::move(string_types)} {
    this->setCurrentIndex(json.value("current").toInt());
  }
};

template<typename TypeList> class OneInTypeCombo : public TypeComboBox {
  struct ForEacher {
    template<typename T> static inline void perform(std::vector<StringType>* string_types) {
      string_types->emplace_back(readableString<T>(), &typeid(T));
    }
  };

  static std::vector<StringType> createStringTypes() {
    std::vector<StringType> output{};
    output.reserve(2 + TypeList::size);
    output.emplace_back("Same as Input", nullptr);
    output.emplace_back("Least Floating-Point", nullptr);
    TypeList::template for_each<ForEacher>(&output);
    return output;
  }

public:
  void updateInputType(const std::type_info* type) {
    if (type) {
      this->update(0, QStringLiteral("Same as Input (") + readableTypeString<TypeList>(*type) + QStringLiteral(")"),
                   type);
      const auto& least_info{least_floating_type_info<TypeList>(*type)};
      this->update(
          1, QStringLiteral("Least Floating-Point (") + readableTypeString<TypeList>(least_info) + QStringLiteral(")"),
          &least_info);
    } else {
      this->update(0, QStringLiteral("Same as Input"), nullptr);
      this->update(1, QStringLiteral("Least Floating-Point"), nullptr);
    }
  }

  OneInTypeCombo() : TypeComboBox{createStringTypes()} {}
  OneInTypeCombo(QJsonObject json) : TypeComboBox{json, createStringTypes()} {}
};

using DefaultOneInTypeCombo = OneInTypeCombo<default_numbers_t>;

template<typename TypeList> class TwoInTypeCombo : public TypeComboBox {
  struct ForEacher {
    template<typename T> static inline void perform(std::vector<StringType>* string_types) {
      string_types->emplace_back(readableString<T>(), &typeid(T));
    }
  };

  const std::type_info *info1_{nullptr}, *info2_{nullptr};

  static std::vector<StringType> createStringTypes() {
    std::vector<StringType> output{};
    output.reserve(4 + TypeList::size);
    output.emplace_back("Same as Input 1", nullptr);
    output.emplace_back("Same as Input 2", nullptr);
    output.emplace_back("Common Input", nullptr);
    output.emplace_back("Least Common Floating-Point", nullptr);
    TypeList::template for_each<ForEacher>(&output);
    return output;
  }

public:
  template<std::size_t Index> void updateInputType(const std::type_info* type) {
    static_assert(Index < 2);
    if constexpr (Index == 0)
      info1_ = type;
    else
      info2_ = type;
    if (info1_)
      this->update(0, QStringLiteral("Same as Input 1 (") + readableTypeString<TypeList>(*info1_) + QStringLiteral(")"),
                   info1_);
    else
      this->update(0, QStringLiteral("Same as Input 1"), nullptr);
    if (info2_)
      this->update(1, QStringLiteral("Same as Input 2 (") + readableTypeString<TypeList>(*info2_) + QStringLiteral(")"),
                   info2_);
    else
      this->update(1, QStringLiteral("Same as Input 2"), nullptr);
    if (info1_ and info2_) {
      const auto& common_info{common_type_info<TypeList, TypeList>(*info1_, *info2_)};
      const auto& least_info{least_floating_type_info<TypeList>(common_info)};
      this->update(2,
                   QStringLiteral("Common Input (") + readableTypeString<TypeList>(common_info) + QStringLiteral(")"),
                   &common_info);
      this->update(3,
                   QStringLiteral("Least Common Floating-Point (") + readableTypeString<TypeList>(least_info) +
                       QStringLiteral(")"),
                   &least_info);
    } else {
      this->update(2, QStringLiteral("Common Input"), nullptr);
      this->update(3, QStringLiteral("Least Common Floating-Point"), nullptr);
    }
  }

  TwoInTypeCombo() : TypeComboBox{createStringTypes()} {}
  TwoInTypeCombo(QJsonObject json) : TypeComboBox{json, createStringTypes()} {}
};

using DefaultTwoInTypeCombo = TwoInTypeCombo<default_numbers_t>;
