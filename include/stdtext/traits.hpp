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

template<>
struct get_character_set_type_of<char>;

template<>
struct get_character_set_type_of<wchar_t>;

template<>
struct get_character_set_type_of<char16_t>;

template<>
struct get_character_set_type_of<char32_t>;

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
