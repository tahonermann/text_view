#if !defined(STDTEXT_CODECS_LITTLE_ENDIAN_CODEC_HPP) // {
#define STDTEXT_CODECS_LITTLE_ENDIAN_CODEC_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/exceptions.hpp>
#include <stdtext/character.hpp>
#include <stdtext/codecs/trivial_codec.hpp>
#include <climits>


namespace std {
namespace experimental {
namespace text {


template<Character CT, Code_unit CUT>
struct little_endian_codec {
    using state_type = trivial_codec_state;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = sizeof(character_type);
    static constexpr int max_code_units = sizeof(character_type);

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
        code_point_type cp = c;

        encoded_code_units = 0;

        for (int i = 0; i < max_code_units; ++i) {
            code_unit_type cu = (cp >> (CHAR_BIT*i)) & ~(unsigned char)0;
            *out++ = cu;
            ++encoded_code_units;
        }
    }

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void rencode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp = c;

        encoded_code_units = 0;

        for (int i = max_code_units; i != 0; --i) {
            code_unit_type cu = (cp >> (CHAR_BIT*(i-1))) & ~(unsigned char)0;
            *out++ = cu;
            ++encoded_code_units;
        }
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
        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp;

        cp = 0;
        decoded_code_units = 0;

        for (int i = 0; i < max_code_units; ++i) {
            if (in_next == in_end) {
                throw text_decode_underflow_error("text decode underflow error");
            } else {
                code_unit_type cu = (unsigned char)*in_next++;
                cp |= (code_point_type)cu << (CHAR_BIT*i);
                decoded_code_units += 1;
            }
        }
        c = cp;
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
        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp;

        cp = 0;
        decoded_code_units = 0;

        for (int i = max_code_units; i != 0; --i) {
            if (in_next == in_end) {
                throw text_decode_underflow_error("text decode underflow error");
            } else {
                code_unit_type cu = (unsigned char)*in_next++;
                cp |= (code_point_type)cu << (CHAR_BIT*(i-1));
                ++decoded_code_units;
            }
        }
        c = cp;
    }
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_LITTLE_ENDIAN_CODEC_HPP
