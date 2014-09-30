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
        *out++ = c;
        encoded_code_units = 1;
    }

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void rencode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        *out++ = c;
        encoded_code_units = 1;
    }

    template<Code_unit_iterator CUIT>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
    static void decode(
        state_type &state,
        CUIT &in_next,
        CUIT in_end,
        character_type &c,
        int &decoded_code_units)
    {
        if (in_next == in_end) {
            throw text_decode_underflow_error("text decode underflow error");
        } else {
            c = *in_next++;
            decoded_code_units = 1;
        }
    }

    template<Code_unit_iterator CUIT>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
    static void rdecode(
        state_type &state,
        CUIT &in_next,
        CUIT in_end,
        character_type &c,
        int &decoded_code_units)
    {
        if (in_next == in_end) {
            throw text_decode_underflow_error("text decode underflow error");
        } else {
            c = *in_next++;
            decoded_code_units = 1;
        }
    }
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_TRIVIAL_CODEC_HPP
