#ifndef STDTEXT_BOUNDED_ITERABLE_HPP // {
#define STDTEXT_BOUNDED_ITERABLE_HPP


#include <origin/sequence/concepts.hpp>


namespace std {
namespace experimental {
namespace text {
namespace detail {

template<origin::Iterator IT, origin::Sentinel<IT> ST>
struct bounded_iterable {
    bounded_iterable()
        : first{}, last{first} {}

    bounded_iterable(IT first, ST last)
        : first{first}, last{last} {}

    IT begin() const { return first; }
    ST end() const { return last; }

private:
    IT first;
    ST last;
};

} // namespace detail
} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_BOUNDED_ITERABLE_HPP
