#pragma once

#include <ImageGraph/internal/Typing.hpp>
#include <QNodeGraph/NodeProxy.hpp>
#include <QtCore/QJsonObject>
#include <QtWidgets/QLayout>
#include <tuple>

template<typename Widget> class DefaultProxy : public NodeProxy {
  Widget* widget_;

  void init() {
    auto* layout{new QHBoxLayout};
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(widget_);
    auto* wrapper{new QWidget};
    wrapper->setLayout(layout);
    setCentralWidget(wrapper);
  }

public:
  QJsonObject toJson() const override { return {{"parent", NodeProxy::toJson()}, {"widget", widget_->toJson()}}; }

  Widget* innerWidget() const { return widget_; }

  DefaultProxy(NodeState state, QGraphicsItem* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{state, parent, flags}, widget_{new Widget{}} {
    init();
  }

  DefaultProxy(QJsonObject json, QGraphicsItem* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags())
      : NodeProxy{json.value("parent").toObject(), parent, flags}, widget_{
                                                                       new Widget{json.value("widget").toObject()}} {
    init();
  }
};
