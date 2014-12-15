#ifndef STDTEXT_ADL_CUSTOMIZATION_HPP // {
#define STDTEXT_ADL_CUSTOMIZATION_HPP


#include <iterator>
#include <utility>


namespace std {
namespace experimental {
namespace text {
namespace detail {

using std::begin;
using std::end;

template<typename C>
auto adl_begin(C &&c) -> decltype(begin(std::forward<C>(c))) {
    return begin(std::forward<C>(c));
}

template<typename C>
auto adl_end(C &&c) -> decltype(end(std::forward<C>(c))) {
    return end(std::forward<C>(c));
}

} // namespace detail
} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_ADL_CUSTOMIZATION_HPP
