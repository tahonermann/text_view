#ifndef STDTEXT_STD_CHARSETS_HPP // {
#define STDTEXT_STD_CHARSETS_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/traits.hpp>


#if !defined(STDTEXT_EXECUTION_CHARACTER_SET)
#define STDTEXT_EXECUTION_CHARACTER_SET \
        basic_execution_character_set
#endif
#if !defined(STDTEXT_EXECUTION_WIDE_CHARACTER_SET)
#define STDTEXT_EXECUTION_WIDE_CHARACTER_SET \
        basic_execution_wide_character_set
#endif
#if !defined(STDTEXT_UNIVERAL_CHARACTER_SET)
#define STDTEXT_UNIVERAL_CHARACTER_SET \
        unicode_character_set
#endif


namespace std {
namespace experimental {
namespace text {


/*
 * C++ basic execution character set
 * ISO/IEC 14882:2011(E) 2.3
 */
struct basic_execution_character_set {
    using code_point_type = char;
};

/*
 * C++ basic execution wide character set
 * ISO/IEC 14882:2011(E) 2.3
 */
struct basic_execution_wide_character_set {
    using code_point_type = wchar_t;
};

/*
 * C++ universal character set
 * ISO/IEC 14882:2011(E) 2.3
 */
template<Code_point CPT>
struct unicode_character_set_template {
    using code_point_type = CPT;
};
using unicode_character_set = unicode_character_set_template<char32_t>;


/*
 * C++ execution character set
 * ISO/IEC 14882:2011(E) 2.3
 */
using execution_character_set = STDTEXT_EXECUTION_CHARACTER_SET;

/*
 * C++ execution wide character set
 * ISO/IEC 14882:2011(E) 2.3
 */
using execution_wide_character_set = STDTEXT_EXECUTION_WIDE_CHARACTER_SET;

/*
 * C++ universal character set
 * ISO/IEC 14882:2011(E) 2.3
 */
using universal_character_set = STDTEXT_UNIVERAL_CHARACTER_SET;


namespace detail {
template<>
struct get_character_set_type_of<char> {
    using type = execution_character_set;
};

template<>
struct get_character_set_type_of<wchar_t> {
    using type = execution_wide_character_set;
};

template<>
struct get_character_set_type_of<char16_t> {
    // Note: char16_t is only capable of holding code points from the BMP.
    using type = universal_character_set;
};

template<>
struct get_character_set_type_of<char32_t> {
    using type = universal_character_set;
};
} // namespace detail


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_STD_CHARSETS_HPP
