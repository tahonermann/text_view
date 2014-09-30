#ifndef STDTEXT_ARCHETYPES_HPP // {
#define STDTEXT_ARCHETYPES_HPP


#include <stdtext/concepts.hpp>


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

    character_archetype_template() : cp(0) {}
    character_archetype_template(code_point_type cp) : cp(cp) {}

    bool operator==(const character_archetype_template& other) {
        return cp == other.cp;
    }
    bool operator!=(const character_archetype_template& other) {
        return cp != other.cp;
    }

    character_archetype_template& operator=(code_point_type) {
        return *this;
    }
    operator code_point_type() const {
        return cp;
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

    template<Code_unit_iterator CUIT>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
    static void decode (
        state_type &state,
        CUIT &in_next,
        CUIT in_end,
        character_type &c,
        int &decoded_code_units);

    template<Code_unit_iterator CUIT>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
    static void rdecode (
        state_type &state,
        CUIT &in_next,
        CUIT in_end,
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
namespace detail {
template<Character_set CST>
struct get_encoding_type_of<character_archetype_template<CST>*> {
    using type = encoding_archetype;
};
} // namespace detail

template<Character_set CST>
using text_iterator_archetype_template = character_archetype_template<CST>*;
using text_iterator_archetype =
          text_iterator_archetype_template<character_set_archetype>;


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
