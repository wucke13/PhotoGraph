#pragma once
#include "Typing.hpp"
#include <QLineEdit>
#include <QValidator>
#include <QWidget>
#include <limits>

namespace PhotoGraph::config {
class QMemoryValidator final : public QValidator {
public:
  QValidator::State validate(QString& input, int&) const final;
};

class MemoryLineEdit : public QLineEdit {
  Q_OBJECT

  std::size_t value_{};

  std::size_t updateValue(const QString& text);
  void newValue(std::size_t value) { emit valueChanged(std::move(value)); }

  MemoryLineEdit(const QString& string, QWidget* parent = nullptr);

signals:
  void valueChanged(std::size_t new_value);

public:
  std::size_t value() const { return value_; }

  MemoryLineEdit(std::size_t memory, QWidget* parent = nullptr);
};

class ImageProcessingPage : public QWidget {
  Q_OBJECT

signals:
  void memoryUpdated(std::size_t);

public:
  ImageProcessingPage(std::size_t memory, QWidget* parent = nullptr);
};
} // namespace PhotoGraph::config
