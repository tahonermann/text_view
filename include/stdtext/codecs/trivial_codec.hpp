#ifndef STDTEXT_CODECS_TRIVIAL_CODEC_HPP // {
#define STDTEXT_CODECS_TRIVIAL_CODEC_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/exceptions.hpp>
#include <stdtext/character.hpp>


namespace std {
namespace experimental {
namespace text {


struct trivial_codec_state {};

template<Character CT, Code_unit CUT>
struct trivial_codec {
    using state_type = trivial_codec_state;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 1;
    static constexpr int max_code_units = 1;

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;

        code_point_type cp{c.get_code_point()};
        code_unit_type cu{cp};
        *out++ = cu;
        encoded_code_units = 1;
    }

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static bool decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;

        if (in_next == in_end) {
            throw text_decode_underflow_error("text decode underflow error");
        } else {
            code_unit_type cu{*in_next++};
            code_point_type cp{cu};
            c.set_code_point(cp);
            decoded_code_units = 1;
        }
        return true;
    }

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static bool rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;

        if (in_next == in_end) {
            throw text_decode_underflow_error("text decode underflow error");
        } else {
            code_unit_type cu{*in_next++};
            code_point_type cp{cu};
            c.set_code_point(cp);
            decoded_code_units = 1;
        }
        return true;
    }
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_TRIVIAL_CODEC_HPP
