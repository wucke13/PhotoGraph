#include <functional>
#include <type_traits>
#include <utility>

/**
 * @brief This requires all types that use it to be implicitly convertible to bool
 *         to represent emptiness and a dereferencing operator that returns the value
 *         if bool(ptr) is true.
 */
template<typename Ptr> struct PtrHash {
  using base_t = std::remove_cvref_t<decltype(*std::declval<Ptr>())>;
  std::size_t operator()(const Ptr& ptr) const { return ptr ? std::hash<base_t>{}(*ptr) : 0; }
};

/**
 * @brief This requires all types that use it to be implicitly convertible to bool
 *         to represent emptiness and a dereferencing operator that returns the value
 *         if bool(ptr) is true.
 */
template<typename Ptr> struct PtrEqual {
  bool operator()(const Ptr& ptr1, const Ptr& ptr2) const {
    return ptr1 and ptr2 ? *ptr1 == *ptr2 : not ptr1 and not ptr2;
  }
};
