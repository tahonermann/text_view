// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_BASIC_VIEW_HPP // {
#define TEXT_VIEW_BASIC_VIEW_HPP


#include <utility>
#include <experimental/ranges/concepts>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<ranges::Iterator IT, ranges::Sentinel<IT> ST = IT>
class basic_view : public ranges::view_base
{
public:
    using iterator = IT;
    using sentinel = ST;

    basic_view() = default;
    basic_view(IT first, ST last)
        : first{first}, last{last} {}

    template<typename IT2, typename ST2>
    requires ranges::Constructible<IT, IT2&&>
          && ranges::Constructible<ST, ST2&&>
    basic_view(IT2 first, ST2 last)
        : first(std::move(first)), last(std::move(last)) {}

    template<typename IT2, typename ST2>
    requires ranges::Constructible<IT, IT2&&>
          && ranges::Constructible<ST, ST2&&>
    basic_view(const basic_view<IT2, ST2> &o)
        : first(o.begin()), last(o.end()) {}

    IT begin() const { return first; }
    ST end() const { return last; }

private:
    IT first = {};
    ST last = {};
};


template<ranges::Iterator IT, ranges::Sentinel<IT> ST>
auto make_basic_view(IT first, ST last) {
    return basic_view<IT, ST>{std::move(first), std::move(last)};
}


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_BASIC_VIEW_HPP
