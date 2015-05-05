#ifndef TEXT_VIEW_STD_CHARSETS_HPP // {
#define TEXT_VIEW_STD_CHARSETS_HPP


#include <text_view_detail/charsets/basic_charsets.hpp>
#include <text_view_detail/charsets/unicode_charsets.hpp>


#if !defined(TEXT_VIEW_EXECUTION_CHARACTER_SET)
#define TEXT_VIEW_EXECUTION_CHARACTER_SET \
        ::std::experimental::text_view::basic_execution_character_set
#endif
#if !defined(TEXT_VIEW_EXECUTION_WIDE_CHARACTER_SET)
#define TEXT_VIEW_EXECUTION_WIDE_CHARACTER_SET \
        ::std::experimental::text_view::basic_execution_wide_character_set
#endif
#if !defined(TEXT_VIEW_UNIVERAL_CHARACTER_SET)
#define TEXT_VIEW_UNIVERAL_CHARACTER_SET \
        ::std::experimental::text_view::unicode_character_set
#endif


namespace std {
namespace experimental {
namespace text_view {


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


} // namespace text_view
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_STD_CHARSETS_HPP
