#include <QSettings>

namespace PhotoGraph::config {
template<typename T> class QSetting {
public:
  enum class SettingsUpdate {
    ON_UPDATE, // Update QSettings when the value is updated
    ON_DESTROY // Update QSettings when QSetting is destroyed
  };

private:
  QSettings& settings_;
  const SettingsUpdate update_;
  const QString key_;
  T value_;

  static T getDefault(QSettings& settings, const QString& key, T def) {
    auto res{settings.value(key)};
    if (res.canConvert<T>())
      return res.value<T>();
    else {
      settings.setValue(key, def);
      return std::move(def);
    }
  }

public:
  QSetting(QSettings& settings, const QString& key, T default_value = T(),
           SettingsUpdate update = SettingsUpdate::ON_DESTROY)
      : settings_{settings}, update_{update}, key_{key}, value_{getDefault(settings, key, default_value)} {}
  ~QSetting() {
    switch (update_) {
      case SettingsUpdate::ON_UPDATE: break;
      case SettingsUpdate::ON_DESTROY: settings_.setValue(key_, value_); break;
    }
  }

  void set(T value) {
    switch (update_) {
      case SettingsUpdate::ON_UPDATE: settings_.setValue(key_, value); break;
      case SettingsUpdate::ON_DESTROY: break;
    }
    value_ = std::move(value);
  }
  const T& get() const { return value_; }
};
} // namespace PhotoGraph::config