// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ADL_CUSTOMIZATION_HPP // {
#define TEXT_VIEW_ADL_CUSTOMIZATION_HPP


#include <iterator>
#include <utility>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {

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

} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ADL_CUSTOMIZATION_HPP
