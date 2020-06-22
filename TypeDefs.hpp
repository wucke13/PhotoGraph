#include <ImageGraph/internal/Typing.hpp>
#include <ImageGraph/internal/typing/NumberTraits.hpp>

using float64_t = double;
using float32_t = float;
using default_numbers_t = ImageGraph::internal::default_numbers_t;

template<typename... Ts> using TypeList = ImageGraph::internal::TypeList<Ts...>;
template<typename... Ts> using TypeListList = ImageGraph::internal::TypeListList<Ts...>;
template<typename T> using least_float_t = ImageGraph::internal::least_floating_point_t<T>;
template<typename T1, typename T2> using common_t = ImageGraph::internal::common_type_t<T1, T2>;
