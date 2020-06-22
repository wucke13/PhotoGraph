#pragma once

#include <QtCore/QJsonObject>
#include <QtWidgets/QCheckBox>

class JsonCheckBox : public QCheckBox {
  Q_OBJECT

public:
  QJsonObject toJson() const { return {{"checked", isChecked()}, {"text", text()}}; }

  explicit JsonCheckBox(QWidget* parent = nullptr) : QCheckBox{parent} {}
  explicit JsonCheckBox(const QString& text, QWidget* parent = nullptr) : QCheckBox{text, parent} {}
  explicit JsonCheckBox(QJsonObject json, QWidget* parent = nullptr)
      : QCheckBox{json.value("text").toString(), parent} {
    setChecked(json.value("checked").toBool());
  }
};
