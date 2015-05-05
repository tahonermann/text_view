#ifndef TEXT_VIEW_RITERATOR_HPP // {
#define TEXT_VIEW_RITERATOR_HPP


#include <text_view_detail/advance_to.hpp>
#include <iterator>
#include <utility>


namespace std {
namespace experimental {
namespace text_view {
namespace detail {

using std::begin;
using std::end;

template<typename C>
auto rbegin(C &&c)
     -> std::reverse_iterator<decltype(begin(std::forward<C>(c)))>
{
    // The std::reverse_iterator constructor is explicit, so the type must be
    // named when constructing the return value.
    using return_type = decltype(rbegin(std::forward<C>(c)));
    return return_type{advance_to(
               begin(std::forward<C>(c)),
               end(std::forward<C>(c)))};
}

template<typename C>
auto rend(C &&c)
     -> std::reverse_iterator<decltype(begin(std::forward<C>(c)))>
{
    // The std::reverse_iterator constructor is explicit, so the type must be
    // named when constructing the return value.
    using return_type = decltype(rend(std::forward<C>(c)));
    return return_type{begin(std::forward<C>(c))};
}

} // namespace detail
} // namespace text_view
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_RITERATOR_HPP
