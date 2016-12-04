// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_BASIC_VIEW_HPP // {
#define TEXT_VIEW_BASIC_VIEW_HPP


#include <experimental/ranges/concepts>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

template<ranges::Iterator IT, ranges::Sentinel<IT> ST = IT>
class basic_view : public ranges::view_base
{
public:
    basic_view() = default;
    basic_view(IT first, ST last)
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


#endif // } TEXT_VIEW_BASIC_VIEW_HPP
