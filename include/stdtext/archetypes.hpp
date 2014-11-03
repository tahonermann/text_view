#ifndef STDTEXT_ARCHETYPES_HPP // {
#define STDTEXT_ARCHETYPES_HPP


#include <stdtext/concepts.hpp>
#include <origin/core/concepts.hpp>
#include <origin/sequence/concepts.hpp>


namespace std {
namespace experimental {
namespace text {


/*
 * Code unit archetype
 */
template<Code_unit CUT>
using code_unit_archetype_template = CUT;
using code_unit_archetype = code_unit_archetype_template<char>;


/*
 * Code point archetype
 */
template<Code_point CPT>
using code_point_archetype_template = CPT;
using code_point_archetype = code_point_archetype_template<unsigned long>;


/*
 * Character set archetype
 */
template<Code_point CPT>
struct character_set_archetype_template
{
    using code_point_type = CPT;
};
using character_set_archetype =
          character_set_archetype_template<code_point_archetype>;


/*
 * Character archetype
 */
template<Character_set CST>
struct character_archetype_template
{
    using character_set_type = CST;
    using code_point_type = typename CST::code_point_type;

    character_archetype_template() : cp{} {}
    character_archetype_template(code_point_type cp) : cp(cp) {}

    bool operator==(const character_archetype_template& other) const noexcept {
        return cp == other.cp;
    }
    bool operator!=(const character_archetype_template& other) const noexcept {
        return !(*this == other);
    }

    void set_code_point(code_point_type cp) noexcept {
        this->cp = cp;
    }
    code_point_type get_code_point() const noexcept {
        return cp;
    }

    static character_set_id get_character_set_id() noexcept {
        return character_set_type::id;
    }

private:
    code_point_type cp;
};
using character_archetype =
          character_archetype_template<character_set_archetype>;


/*
 * Code unit iterator archetype
 */
template<Code_unit CUT>
using code_unit_iterator_archetype_template = CUT*;
using code_unit_iterator_archetype =
          code_unit_iterator_archetype_template<code_unit_archetype>;


/*
 * Codec state archetype
 */
struct codec_state_archetype {};


/*
 * Codec archetype
 */
template <
    Codec_state CST,
    Code_unit CUT,
    Character C,
    int MinCodeUnits = 1,
    int MaxCodeUnits = 1>
struct codec_archetype_template {
    using state_type = CST;
    using code_unit_type = CUT;
    using character_type = C;
    static constexpr int min_code_units = MinCodeUnits;
    static constexpr int max_code_units = MaxCodeUnits;

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode (
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units);

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void rencode (
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units);

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static void decode (
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units);

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static void rdecode (
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units);
};
using codec_archetype = codec_archetype_template<
                            codec_state_archetype,
                            code_unit_archetype,
                            character_archetype>;


/*
 * Encoding archetype
 */
template<Codec CT>
struct encoding_archetype_template
{
    using codec_type = CT;
    static typename codec_type::state_type initial_state;
};
using encoding_archetype = encoding_archetype_template<codec_archetype>;


/*
 * Text iterator archetype
 */
template<Encoding E, Code_unit_iterator CUIT>
struct text_iterator_archetype_template {
    using encoding_type = E;
    using state_type = typename E::codec_type::state_type;
    using iterator = CUIT;
    using iterator_category = origin::Iterator_category<CUIT>;
    using value_type = typename E::codec_type::character_type;
    using reference = typename E::codec_type::character_type&;
    using pointer = typename E::codec_type::character_type*;
    using difference_type = origin::Difference_type<CUIT>;

    text_iterator_archetype_template();
    text_iterator_archetype_template(iterator, iterator);
    text_iterator_archetype_template(state_type, iterator, iterator);
    const state_type& state() const;
    state_type& state();
    iterator base() const;
    iterator begin() const;
    iterator end() const;
    reference operator*() const;
    pointer operator->() const;
    bool operator==(const text_iterator_archetype_template& other) const;
    bool operator!=(const text_iterator_archetype_template& other) const;
    bool operator<(const text_iterator_archetype_template& other) const;
    bool operator>(const text_iterator_archetype_template& other) const;
    bool operator<=(const text_iterator_archetype_template& other) const;
    bool operator>=(const text_iterator_archetype_template& other) const;
    text_iterator_archetype_template& operator++();
    text_iterator_archetype_template operator++(int);
    text_iterator_archetype_template& operator--();
    text_iterator_archetype_template operator--(int);
    text_iterator_archetype_template& operator+=(difference_type n);
    text_iterator_archetype_template operator+(difference_type n) const;
    text_iterator_archetype_template& operator-=(difference_type n);
    text_iterator_archetype_template operator-(difference_type n) const;
    difference_type operator-(text_iterator_archetype_template it) const;
    value_type operator[](difference_type n) const;
};
template<Encoding E, Code_unit_iterator CUIT>
text_iterator_archetype_template<E, CUIT> operator+(
    origin::Difference_type<CUIT> n,
    text_iterator_archetype_template<E, CUIT> it);
using text_iterator_archetype = text_iterator_archetype_template<
                                    encoding_archetype,
                                    code_unit_iterator_archetype>;


/*
 * Text view archetype
 */
template<Code_unit CUT, unsigned N>
using text_view_archetype_template = CUT[N];
using text_view_archetype = text_view_archetype_template<code_unit_archetype, 5>;


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_ARCHETYPES_HPP
