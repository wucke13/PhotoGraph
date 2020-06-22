#pragma once
#include "TypeDefs.hpp"
#include <ImageGraph/core/Rectangle.hpp>
#include <ImageGraph/core/Tile.hpp>
#include <QNodeGraph/helpers/Typing.hpp>
#include <QtCore/QMetaType>
#include <QtCore/QRectF>
#include <QtCore/QString>
#include <memory>
#include <optional>
#include <utility>

using tile_u8_t = std::shared_ptr<ImageGraph::Tile<uint8_t>>;
Q_DECLARE_METATYPE(tile_u8_t)

using optional_double_t = std::optional<double>;
Q_DECLARE_METATYPE(optional_double_t)
using optional_size_t = std::optional<std::size_t>;
Q_DECLARE_METATYPE(optional_size_t)

struct type_info {
  const std::type_info* info{nullptr};
  type_info() = default;
  type_info(const std::type_info* info) : info{info} {}
};
Q_DECLARE_METATYPE(type_info)

namespace {
template<typename T> struct QStringConverter {};

template<> struct QStringConverter<double> {
  static inline double convert(const QString& source, bool& ok) { return source.toDouble(&ok); }
};
template<> struct QStringConverter<int> {
  static inline int convert(const QString& source, bool& ok, int base = 0) { return source.toInt(&ok, base); }
};
template<> struct QStringConverter<unsigned> {
  static inline unsigned convert(const QString& source, bool& ok, int base = 0) { return source.toUInt(&ok, base); }
};

template<> struct QStringConverter<unsigned long> {
  static inline unsigned long convert(const QString& source, bool& ok, int base = 0) {
    return source.toULong(&ok, base);
  }
};
} // namespace

template<typename T, typename... Args>
static inline std::optional<T> q_string_convert(const QString& source, Args&&... args) {
  bool ok{false};
  T value{QStringConverter<T>::convert(source, ok, std::forward<Args>(args)...)};
  return ok ? std::make_optional(value) : std::nullopt;
}

namespace {
struct StringPerformer {
  template<typename T> static QString perform() { return readableString<T>(); }
};
} // namespace
template<typename TypeList> static inline QString readableTypeString(const std::type_info& info) {
  return TypeList::template perform<StringPerformer, QString>(info).value();
}

template<typename T> QRectF fromRect(const ImageGraph::Rectangle<T>& rect) {
  return {qreal(rect.point().x()), qreal(rect.point().y()), qreal(rect.dimensions().width()),
          qreal(rect.dimensions().height())};
}

namespace {
struct LeastFloatPerformer {
  template<typename T> static const std::type_info* perform() { return &typeid(least_float_t<T>); }
};
} // namespace
template<typename TypeList>
static inline const std::type_info& least_floating_type_info(const std::type_info& type_info) {
  return *TypeList::template perform<LeastFloatPerformer, const std::type_info*>(type_info).value();
}

namespace {
struct CommonPerformer {
  template<typename T1, typename T2> static const std::type_info* perform() { return &typeid(common_t<T1, T2>); }
};
} // namespace
template<typename TypeList1, typename TypeList2>
static inline const std::type_info& common_type_info(const std::type_info& info1, const std::type_info& info2) {
  return *TypeListList<TypeList1, TypeList2>::template perform<CommonPerformer, const std::type_info*>(
              std::tie(info1, info2))
              .value();
}
