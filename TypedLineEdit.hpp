#pragma once
#include "Typing.hpp"
#include <QtCore/QJsonObject>
#include <QtWidgets/QLineEdit>

template<typename T> class TypedLineEdit : public QLineEdit {
  std::optional<T> value_{};

  void setup() {
    connect(this, &QLineEdit::editingFinished, [this] {
      auto value{updateValue()};
      if (value != value_) {
        value_ = std::move(value);
        newValue(value_);
      }
    });
  }

protected:
  std::optional<T> updateValue() { return q_string_convert<T>(text()); }
  virtual void newValue(std::optional<T>) = 0;

public:
  std::optional<T> value() const { return value_; }

  virtual QJsonObject toJson() const { return {{"text", this->text()}}; }

  explicit TypedLineEdit(QWidget* parent = nullptr) : QLineEdit{parent} { setup(); }
  TypedLineEdit(const QString& string, QWidget* parent = nullptr) : QLineEdit{string, parent} {
    setup();
    value_ = updateValue();
  }
  TypedLineEdit(QJsonObject json, QWidget* parent = nullptr) : TypedLineEdit{json.value("text").toString(), parent} {}
};

class DoubleLineEdit : public TypedLineEdit<double> {
  Q_OBJECT

  void newValue(optional_double_t value) { emit valueChanged(std::move(value)); }

signals:
  void valueChanged(optional_double_t new_value);

public:
  DoubleLineEdit(QWidget* parent = nullptr) : TypedLineEdit<double>(parent) {}
  DoubleLineEdit(QJsonObject json, QWidget* parent = nullptr) : TypedLineEdit<double>(json, parent) {}
};

class SizeTypeLineEdit : public TypedLineEdit<std::size_t> {
  Q_OBJECT

  void newValue(optional_size_t value) { emit valueChanged(std::move(value)); }

signals:
  void valueChanged(optional_size_t new_value);

public:
  SizeTypeLineEdit(QWidget* parent = nullptr) : TypedLineEdit<std::size_t>(parent) {}
  SizeTypeLineEdit(QJsonObject json, QWidget* parent = nullptr) : TypedLineEdit<std::size_t>(json, parent) {}
};
