// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_BOUNDED_ITERABLE_HPP // {
#define TEXT_VIEW_BOUNDED_ITERABLE_HPP


#include <origin/algorithm/concepts.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<origin::Iterator IT, origin::Sentinel<IT> ST>
class bounded_iterable {
public:
    bounded_iterable() = default;
    bounded_iterable(IT first, ST last)
        : first{first}, last{last} {}

    IT begin() const { return first; }
    ST end() const { return last; }

private:
    IT first = {};
    ST last = {};
};

} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_BOUNDED_ITERABLE_HPP
