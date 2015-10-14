// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_STD_CHARSETS_HPP // {
#define TEXT_VIEW_STD_CHARSETS_HPP


#include <text_view_detail/charsets/basic_charsets.hpp>
#include <text_view_detail/charsets/unicode_charsets.hpp>


#if !defined(TEXT_VIEW_EXECUTION_CHARACTER_SET)
#define TEXT_VIEW_EXECUTION_CHARACTER_SET \
        ::std::experimental::basic_execution_character_set
#endif
#if !defined(TEXT_VIEW_EXECUTION_WIDE_CHARACTER_SET)
#define TEXT_VIEW_EXECUTION_WIDE_CHARACTER_SET \
        ::std::experimental::basic_execution_wide_character_set
#endif
#if !defined(TEXT_VIEW_UNIVERAL_CHARACTER_SET)
#define TEXT_VIEW_UNIVERAL_CHARACTER_SET \
        ::std::experimental::unicode_character_set
#endif


namespace std {
namespace experimental {
inline namespace text {


/*
 * C++ execution character set
 * ISO/IEC 14882:2011(E) 2.3
 */
using execution_character_set = TEXT_VIEW_EXECUTION_CHARACTER_SET;

/*
 * C++ execution wide character set
 * ISO/IEC 14882:2011(E) 2.3
 */
using execution_wide_character_set = TEXT_VIEW_EXECUTION_WIDE_CHARACTER_SET;

/*
 * C++ universal character set
 * ISO/IEC 14882:2011(E) 2.3
 */
using universal_character_set = TEXT_VIEW_UNIVERAL_CHARACTER_SET;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_STD_CHARSETS_HPP
