#pragma once
#include <QtCore/QMetaObject>
#include <QtCore/QObject>
#include <iostream>

class ConnectionWrapper {
  QMetaObject::Connection connection_;

public:
  const QMetaObject::Connection& connection() const { return connection_; }

  explicit ConnectionWrapper(QMetaObject::Connection connection) : connection_{std::move(connection)} {}
  ConnectionWrapper(ConnectionWrapper&& connection) = delete;
  ConnectionWrapper& operator=(ConnectionWrapper&&) = delete;
  ConnectionWrapper(const ConnectionWrapper&) = delete;
  ConnectionWrapper& operator=(const ConnectionWrapper&) = delete;
  ~ConnectionWrapper() { QObject::disconnect(connection_); }
};
