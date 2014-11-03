#ifndef STDTEXT_CONCEPTS_HPP // {
#define STDTEXT_CONCEPTS_HPP


#include <origin/core/traits.hpp>
#include <origin/sequence/concepts.hpp>
#include <origin/sequence/range.hpp>
#include <stdtext/traits.hpp>


namespace std {
namespace experimental {
namespace text {


namespace detail {
template<typename T, T t1, T t2>
concept bool Same_value() {
    return t1 == t2;
}
} // detail namespace


/*
 * Code unit concept
 */
template<typename T>
concept bool Code_unit() {
    return origin::Integral_type<T>()
        && (origin::Unsigned_type<T>()
            || origin::Same<T, origin::Remove_cv<char>>()
            || origin::Same<T, origin::Remove_cv<wchar_t>>());
}


/*
 * Code point concept
 */
template<typename T>
concept bool Code_point() {
    return origin::Integral_type<T>()
        && (origin::Unsigned_type<T>()
            || origin::Same<T, origin::Remove_cv<char>>()
            || origin::Same<T, origin::Remove_cv<wchar_t>>());
}


/*
 * Character set concept
 */
template<typename T>
concept bool Character_set() {
    return requires () {
               typename T::code_point_type;
           }
        && Code_point<typename T::code_point_type>();
}


/*
 * Character concept
 */
template<typename T>
concept bool Character() {
    return Character_set<character_set_type_of<T>>()
        && origin::Regular<T>()
        && origin::Copy_assignable<T>()
        && requires (T t, code_point_type_of<character_set_type_of<T>> cp) {
               t.set_code_point(cp);
               { t.get_code_point() } -> code_point_type_of<character_set_type_of<T>>;
               { t.get_character_set_id() } -> character_set_id;
           };
}


/*
 * Code unit iterator concept
 */
template<typename T>
concept bool Code_unit_iterator() {
    return origin::Iterator<T>()
        && Code_unit<origin::Value_type<T>>();
}


/*
 * Codec state concept
 * These requirements are intended to match the char_traits<T>::state_type
 * requirements described in C++11 [char.traits.typedefs] 21.2.2p4.
 */
template<typename T>
concept bool Codec_state() {
    return origin::Default_constructible<T>()
        && origin::Copy_constructible<T>()
        && origin::Copy_assignable<T>();
}


/*
 * Codec concept
 */
template<typename T>
concept bool Codec() {
    return requires () {
               typename T::state_type;
               typename T::code_unit_type;
               typename T::character_type;
               // FIXME: gcc rejects the constexpr requirement.
               /* constexpr */ { T::min_code_units } noexcept -> int;
               /* constexpr */ { T::max_code_units } noexcept -> int;
           }
        && Codec_state<typename T::state_type>()
        && Code_unit<typename T::code_unit_type>()
        && Character<typename T::character_type>()
        // FIXME: Move the requirement for an encode() member function to a
        // FIXME: different concept that validates a particular iterator type?
        && requires (
               typename T::state_type &state,
               typename T::code_unit_type *&out,
               typename T::character_type c,
               int &encoded_code_units)
           {
               T::encode(state, out, c, encoded_code_units);
           }
        // FIXME: Move the requirement for a decode() member function to a
        // FIXME: different concept that validates a particular iterator type?
        && requires (
               typename T::state_type &state,
               typename T::code_unit_type *&in_next,
               typename T::code_unit_type *in_end,
               typename T::character_type &c,
               int &decoded_code_units)
           {
               T::decode(state, in_next, in_end, c, decoded_code_units);
           };
}


/*
 * Bidirectional codec concept
 */
template<typename T>
concept bool Bidirectional_codec() {
    return Codec<T>()
        // FIXME: Move the requirement for a rencode() member function to a
        // FIXME: different concept that validates a particular iterator type?
        && requires (
               typename T::state_type &state,
               typename T::code_unit_type *&out,
               typename T::character_type c,
               int &encoded_code_units)
           {
               T::rencode(state, out, c, encoded_code_units);
           }
        // FIXME: Move the requirement for a rdecode() member function to a
        // FIXME: different concept that validates a particular iterator type?
        && requires (
               typename T::state_type &state,
               typename T::code_unit_type *&in_next,
               typename T::code_unit_type *in_end,
               typename T::character_type &c,
               int &decoded_code_units)
           {
               T::rdecode(state, in_next, in_end, c, decoded_code_units);
           };
}


/*
 * Random access codec concept
 */
template<typename T>
concept bool Random_access_codec() {
    return Bidirectional_codec<T>()
        && detail::Same_value<int, T::min_code_units, T::max_code_units>()
        && origin::Empty_type<typename T::state_type>();
}


/*
 * Encoding concept
 */
template<typename T>
concept bool Encoding() {
    return requires () {
               typename T::codec_type;
           }
        && Codec<typename T::codec_type>()
        && requires () {
               { T::initial_state } noexcept
                   -> const typename T::codec_type::state_type&;
           };
}


/*
 * Text iterator concept
 */
template<typename T>
concept bool Text_iterator() {
    return Encoding<encoding_type_of<T>>()
        && origin::Iterator<T>()
        && Character<origin::Value_type<T>>()
        && requires (T t, const T ct) {
               { t.state() }
                   -> typename encoding_type_of<T>::codec_type::state_type&;
               { ct.state() }
                   -> const typename encoding_type_of<T>::codec_type::state_type&;
           };
}


/*
 * Text view concept
 */
template<typename T>
concept bool Text_view() {
    return Encoding<encoding_type_of<T>>()
        && origin::Input_range<T>();
}


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CONCEPTS_HPP
