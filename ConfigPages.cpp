#include "ConfigPages.hpp"
#include "Typing.hpp"
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <qboxlayout.h>

using namespace PhotoGraph::config;

constexpr std::size_t factor{1ul << 20}, maximum{std::numeric_limits<std::size_t>::max() / factor};

QValidator::State QMemoryValidator::validate(QString& input, int&) const {
  auto value{q_string_convert<std::size_t>(input)};
  if (value) {
    if (*value < maximum)
      return QValidator::State::Acceptable;
    else
      return QValidator::State::Intermediate;
  } else
    return QValidator::State::Invalid;
}

std::size_t MemoryLineEdit::updateValue(const QString& text) {
  return q_string_convert<std::size_t>(text).value() * factor;
}

MemoryLineEdit::MemoryLineEdit(const QString& string, QWidget* parent) : QLineEdit{string, parent} {
  setValidator(new QMemoryValidator);
  connect(this, &QLineEdit::editingFinished, [this] {
    auto value{updateValue(text())};
    if (value != value_) {
      value_ = value;
      newValue(value);
    }
  });
  value_ = updateValue(string);
}
MemoryLineEdit::MemoryLineEdit(std::size_t memory, QWidget* parent)
    : MemoryLineEdit{QString::number(memory / factor), parent} {}

ImageProcessingPage::ImageProcessingPage(std::size_t memory, QWidget* parent) : QWidget(parent) {
  auto* memory_group{new QGroupBox(tr("Memory Configuration"))};

  auto* memory_line{new MemoryLineEdit(memory)};
  connect(memory_line, &MemoryLineEdit::valueChanged, [this](std::size_t value) { emit memoryUpdated(value); });

  auto* memory_layout{new QFormLayout};
  memory_layout->addRow(tr("Amount of memory for caching (in MiB):"), memory_line);
  memory_group->setLayout(memory_layout);

  auto* main_layout{new QVBoxLayout};
  main_layout->addWidget(memory_group);

  setLayout(main_layout);
}
