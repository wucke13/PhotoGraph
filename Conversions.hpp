#include <QNodeGraph/helpers/Conversions.hpp>

namespace json_convert {
template<typename T> struct Converter<std::optional<T>> {
  static inline QJsonValue to(const std::optional<T>& optional) {
    if (optional.has_value())
      return QJsonObject{{"value", Converter<T>::to(optional.value())}};
    else
      return QJsonObject{};
  }
  static inline std::optional<T> from(QJsonValue value) {
    auto json{value.toObject().value("value")};
    if (json.isObject())
      return Converter<T>::from(json);
    else
      return std::nullopt;
  }
};
} // namespace json_convert
