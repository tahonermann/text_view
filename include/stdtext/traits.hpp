#ifndef STDTEXT_TRAITS_HPP // {
#define STDTEXT_TRAITS_HPP


#include <origin/core/traits.hpp>
#include <origin/sequence/concepts.hpp>
#include <origin/sequence/range.hpp>


namespace std {
namespace experimental {
namespace text {


/*
 * Associated code point type helper
 */
namespace detail {
template<typename T>
struct get_code_point_type_of;

template<typename T>
requires requires () { typename T::code_point_type; }
struct get_code_point_type_of<T> {
    using type = typename T::code_point_type;
};

template<typename T>
using code_point_type_of =
          typename get_code_point_type_of<origin::Strip<T>>::type;
} // namespace detail

template<typename T>
using code_point_type_of = detail::code_point_type_of<T>;


/*
 * Associated character set type helper
 */
namespace detail {
template<typename T>
struct get_character_set_type_of;

template<typename T>
requires requires () { typename T::character_set_type; }
struct get_character_set_type_of<T> {
    using type = typename T::character_set_type;
};

template<typename T>
using character_set_type_of =
          typename get_character_set_type_of<origin::Strip<T>>::type;
} // namespace detail

template<typename T>
using character_set_type_of = detail::character_set_type_of<T>;


/*
 * Associated encoding type helper
 */
namespace detail {
template<typename T>
struct get_encoding_type_of;

// String and character literals have statically known execution character
// encodings (though the literal code units need not form validly encoded
// text).  The underlying types of string and character literals
// (const char[N], char, wchar_t, int (for multicharacter literals), etc...)
// do not unambiguously imply a particular encoding since these types can be
// used as the underlying storage for arbitrary data).  It is not currently
// possible to differentiate between a string or character literal and
// variables declared using the underlying type.  If it were, then it would
// be possible to infer an encoding for string and character literals, but
// not for variables of the underlying types.  Since it isn't, a choice
// must be made to either infer encodings based on these underlying types
// (allowing the user to override the inference), or require the user to
// always specify an encoding.  For convenience, and since the underlying
// types are frequently used for storing text that does use the same
// encodings as string literals, types corresponding to such string literals
// (including their decayed types) are assumed by encoding_type_of to have
// an associated encoding matching a string literal of that type.  Note
// that this assumption is not extended to the underlying types of
// character literals by character_set_type_of since values of these types
// may not be capable of storing all code points valid for the encoded
// character sets.
template<>
struct get_encoding_type_of<char*>;
template<>
struct get_encoding_type_of<wchar_t*>;
template<>
struct get_encoding_type_of<char16_t*>;
template<>
struct get_encoding_type_of<char32_t*>;

template<typename T>
struct get_encoding_type_of<const T*> {
    using type = typename get_encoding_type_of<T*>::type;
};

template<typename T>
struct get_encoding_type_of<T[]> {
    using type = typename get_encoding_type_of<T*>::type;
};

template<typename T, unsigned N>
struct get_encoding_type_of<T[N]> {
    using type = typename get_encoding_type_of<T*>::type;
};

template<typename T>
requires requires () { typename T::encoding_type; }
struct get_encoding_type_of<T> {
    using type = typename T::encoding_type;
};

template<typename T>
using encoding_type_of = typename get_encoding_type_of<origin::Strip<T>>::type;
} // namespace detail

template<typename T>
using encoding_type_of = detail::encoding_type_of<T>;


/*
 * Associated code unit iterator type helper
 */
namespace detail {
template<typename T>
struct get_code_unit_iterator_type_of;

template<typename T>
struct get_code_unit_iterator_type_of<T*> {
    using type = T*;
};

template<typename T>
requires requires () { typename T::code_unit_iterator_type; }
struct get_code_unit_iterator_type_of<T> {
    using type = typename T::code_unit_iterator_type;
};

template<typename T>
using code_unit_iterator_type_of =
          typename get_code_unit_iterator_type_of<origin::Strip<T>>::type;
} // namespace detail

template<typename T>
using code_unit_iterator_type_of = detail::code_unit_iterator_type_of<T>;


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_TRAITS_HPP
