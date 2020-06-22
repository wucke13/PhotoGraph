#pragma once
#include "JsonWidgets.hpp"
#include "TypeComboBox.hpp"
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtWidgets/QFormLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QWidget>
#include <utility>

template<typename TypeList> class LabelValueGrid : public QWidget {
public:
  template<typename Widget> struct LabeledWidget {
    QLabel* label{};
    Widget* widget{nullptr};

    QJsonObject toJson() const { return {{"label", label->text()}, {"widget", widget->toJson()}}; }

    LabeledWidget(const QString& label, Widget* widget) : label{new QLabel{label}}, widget{widget} {}
    LabeledWidget(const QString& label) : LabeledWidget{std::move(label), new Widget{}} {}
    LabeledWidget(QJsonObject json)
        : LabeledWidget{json.value("label").toString(), new Widget{json.value("widget").toObject()}} {}
  };

protected:
  struct Transformer {
    template<typename T> using type = LabeledWidget<T>;
  };

public:
  using tuple_t = typename TypeList::template transformed_t<Transformer>::tuple_t;
  template<std::size_t Index> using at_t = typename TypeList::template at_t<Index>;
  template<std::size_t Index> using pair_at_t = LabeledWidget<at_t<Index>>;

private:
  template<std::size_t Index = 0> static inline void addRows(QFormLayout& layout, tuple_t& tuple) {
    if constexpr (Index == std::tuple_size_v<tuple_t>)
      return;
    else {
      pair_at_t<Index>& pair{std::get<Index>(tuple)};
      layout.addRow(pair.label, pair.widget);
      addRows<Index + 1>(layout, tuple);
    }
  }
  template<std::size_t Index = 0> static inline void addJson(QJsonArray& json, const tuple_t& tuple) {
    if constexpr (Index == std::tuple_size_v<tuple_t>)
      return;
    else {
      json.append(std::get<Index>(tuple).toJson());
      addJson<Index + 1>(json, tuple);
    }
  }
  template<typename IndexSequence> struct Jsoner {};
  template<std::size_t... Indices> struct Jsoner<std::index_sequence<Indices...>> {
    template<std::size_t Index> static inline pair_at_t<Index> create(const QJsonArray& json) {
      return {json.at(Index).toObject()};
    }
    static inline tuple_t map(const QJsonArray& json) { return tuple_t{create<Indices>(json)...}; }
  };
  using DefaultJsoner = Jsoner<std::make_index_sequence<TypeList::size>>;

  void init() {
    addRows(*layout_, pairs_);
    setLayout(layout_);
    setContentsMargins(0, 0, 0, 0);
  }

  QFormLayout* layout_;
  tuple_t pairs_;

public:
  QJsonObject toJson() const {
    QJsonArray array{};
    addJson(array, pairs_);
    return {{"pairs", array}};
  }

  template<std::size_t Index> const pair_at_t<Index>& get() const { return std::get<Index>(pairs_); }
  template<std::size_t Index> pair_at_t<Index>& get() { return std::get<Index>(pairs_); }

  LabelValueGrid(tuple_t pairs, QWidget* parent = nullptr)
      : QWidget{parent}, layout_{new QFormLayout{this}}, pairs_{std::move(pairs)} {
    init();
  }
  LabelValueGrid(QJsonObject json, QWidget* parent = nullptr)
      : QWidget{parent}, layout_{new QFormLayout{this}}, pairs_{DefaultJsoner::map(json.value("pairs").toArray())} {
    init();
  }
  LabelValueGrid(const LabelValueGrid&) = delete;
  LabelValueGrid& operator=(const LabelValueGrid&) = delete;
};

template<typename Types> class DefaultTypedList
    : public LabelValueGrid<typename Types::template combined_t<TypeList<JsonCheckBox, DefaultOneInTypeCombo>>> {
  using type_list = typename Types::template combined_t<TypeList<JsonCheckBox, DefaultOneInTypeCombo>>;
  using parent_t = LabelValueGrid<type_list>;
  using tuple_t = typename Types::template transformed_t<typename parent_t::Transformer>::tuple_t;
  using check_t = typename parent_t::template LabeledWidget<JsonCheckBox>;
  using combo_t = typename parent_t::template LabeledWidget<DefaultOneInTypeCombo>;

public:
  JsonCheckBox* ditherBox() const { return this->template get<type_list::size - 2>().widget; }
  DefaultOneInTypeCombo* typeCombo() const { return this->template get<type_list::size - 1>().widget; }

  DefaultTypedList(tuple_t pairs, QWidget* parent = nullptr)
      : parent_t{std::tuple_cat(std::move(pairs), std::make_tuple(check_t{"Dither"}, combo_t{"Output Type"})), parent} {
  }
  DefaultTypedList(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};

template<typename Types> class TwoInTypedList
    : public LabelValueGrid<typename Types::template combined_t<TypeList<JsonCheckBox, DefaultTwoInTypeCombo>>> {
  using type_list = typename Types::template combined_t<TypeList<JsonCheckBox, DefaultTwoInTypeCombo>>;
  using parent_t = LabelValueGrid<type_list>;
  using tuple_t = typename Types::template transformed_t<typename parent_t::Transformer>::tuple_t;
  using check_t = typename parent_t::template LabeledWidget<JsonCheckBox>;
  using combo_t = typename parent_t::template LabeledWidget<DefaultTwoInTypeCombo>;

public:
  JsonCheckBox* ditherBox() const { return this->template get<type_list::size - 2>().widget; }
  DefaultTwoInTypeCombo* typeCombo() const { return this->template get<type_list::size - 1>().widget; }

  TwoInTypedList(tuple_t pairs, QWidget* parent = nullptr)
      : parent_t{std::tuple_cat(std::move(pairs), std::make_tuple(check_t{"Dither"}, combo_t{"Output Type"})), parent} {
  }
  TwoInTypedList(QJsonObject json, QWidget* parent = nullptr) : parent_t{json, parent} {}
};
