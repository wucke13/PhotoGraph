#pragma once

#include <memory>
#include <ostream>
#include <type_traits>
#include <unordered_set>
#include <vector>

class ActionQueue;

namespace {
template<typename T> struct is_container { constexpr static bool value{false}; };
template<typename T, typename Allocator> struct is_container<std::vector<T, Allocator>> {
  constexpr static bool value{true};
};
template<typename Key, typename Hash, typename KeyEqual, class Allocator>
struct is_container<std::unordered_set<Key, Hash, KeyEqual, Allocator>> {
  constexpr static bool value{true};
};
template<> struct is_container<ActionQueue> { constexpr static bool value{true}; };

template<typename T> constexpr static bool is_container_v = is_container<T>::value;

// Automatic dereferencing of pointers when printing.
template<typename T> struct PrintDereferencer {
  static inline std::ostream& stream(std::ostream& stream, const T& value) { return stream << value; }
};
template<typename T> struct PrintDereferencer<T*> {
  static inline std::ostream& stream(std::ostream& stream, const T* value) {
    if (value) {
      stream << "&(";
      PrintDereferencer<T>::stream(stream, *value);
      stream << ")";
    } else
      stream << "nullptr";
    return stream;
  }
};
template<typename T> struct PrintDereferencer<std::unique_ptr<T>> {
  static inline std::ostream& stream(std::ostream& stream, const std::unique_ptr<T>& value) {
    if (value) {
      stream << "std::unique_ptr(";
      PrintDereferencer<T>::stream(stream, *value);
      stream << ")";
    } else
      stream << "nullptr";
    return stream;
  }
};
template<typename T> static inline std::ostream& print_dereferenced(std::ostream& stream, const T& value) {
  return PrintDereferencer<T>::stream(stream, value);
}
} // namespace

template<typename Cont>
std::enable_if_t<is_container_v<Cont>, std::ostream&> operator<<(std::ostream& stream, const Cont& container) {
  stream << "{";
  for (auto it{container.begin()}; it != container.end();) {
    print_dereferenced(stream, *it);
    ++it;
    if (it != container.end()) stream << ", ";
  }
  stream << "}";
  return stream;
}
