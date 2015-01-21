#if !defined(STDTEXT_CODECS_UTF16_CODEC_HPP) // {
#define STDTEXT_CODECS_UTF16_CODEC_HPP


#include <stdtext/concepts.hpp>
#include <stdtext/exceptions.hpp>
#include <stdtext/character.hpp>
#include <stdtext/codecs/trivial_codec.hpp>
#include <climits>


namespace std {
namespace experimental {
namespace text {


template<Character CT, Code_unit CUT>
struct utf16_codec {
    using state_type = trivial_codec_state;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 1;
    static constexpr int max_code_units = 2;

    static_assert(sizeof(code_unit_type) * CHAR_BIT >= 16, "");

    template<Code_unit_iterator CUIT>
    requires origin::Output_iterator<CUIT, code_unit_type>()
    static void encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp{c.get_code_point()};

        if (cp >= 0xD800 && cp <= 0xDFFF) {
            throw text_encode_error("Invalid Unicode code point");
        }

        if (cp < 0xFFFF) {
            *out++ = code_unit_type(cp);
            ++encoded_code_units;
        } else {
            *out++ = code_unit_type(0xD800 + (((cp - 0x10000) >> 10) & 0x03FF));
            ++encoded_code_units;
            *out++ = code_unit_type(0xDC00 + ((cp - 0x10000) & 0x03FF));
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
        encoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp{c.get_code_point()};

        if (cp >= 0xD800 && cp <= 0xDFFF) {
            throw text_encode_error("Invalid Unicode code point");
        }

        if (cp < 0xFFFF) {
            *out++ = code_unit_type(cp);
            ++encoded_code_units;
        } else {
            *out++ = code_unit_type(0xDC00 + ((cp - 0x10000) & 0x03FF));
            ++encoded_code_units;
            *out++ = code_unit_type(0xD800 + (((cp - 0x10000) >> 10) & 0x03FF));
            ++encoded_code_units;
        }
    }

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static void decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp;

        if (in_next == in_end)
            throw text_decode_underflow_error("text decode underflow error");
        code_unit_type cu1 = *in_next++;
        ++decoded_code_units;
        if (cu1 >= 0xD800 && cu1 <= 0xDBFF) {
            if (in_next == in_end)
                throw text_decode_underflow_error("text decode underflow error");
            code_unit_type cu2 = *in_next++;
            ++decoded_code_units;
            if (cu2 < 0xDC00 || cu2 > 0xDFFF) {
                throw text_decode_error("Invalid UTF-16 code unit sequence");
            }
            cp = 0x10000 + (((cu1 & 0x3FF) << 10) | (cu2 & 0x3FF));
            c.set_code_point(cp);
        } else if (cu1 >= 0xDC00 && cu1 <= 0xDFFF) {
            throw text_decode_error("Invalid UTF-16 code unit sequence");
        } else {
            cp = cu1;
            c.set_code_point(cp);
        }
    }

    template<Code_unit_iterator CUIT, typename CUST>
    requires origin::Input_iterator<CUIT>()
          && origin::Convertible<origin::Value_type<CUIT>, code_unit_type>()
          && origin::Sentinel<CUST, CUIT>()
    static void rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        using code_point_type =
            code_point_type_of<character_set_type_of<character_type>>;
        code_point_type cp;

        if (in_next == in_end)
            throw text_decode_underflow_error("text decode underflow error");
        code_unit_type rcu1 = *in_next++;
        ++decoded_code_units;
        if (rcu1 >= 0xDC00 && rcu1 <= 0xDFFF) {
            if (in_next == in_end)
                throw text_decode_underflow_error("text decode underflow error");
            code_unit_type rcu2 = *in_next++;
            ++decoded_code_units;
            if (rcu2 < 0xD800 || rcu2 > 0xDBFF) {
                throw text_decode_error("Invalid UTF-16 code unit sequence");
            }
            cp = 0x10000 + (((rcu2 & 0x3FF) << 10) | (rcu1 & 0x3FF));
            c.set_code_point(cp);
        } else if (rcu1 >= 0xD800 && rcu1 <= 0xDBFF) {
            throw text_decode_error("Invalid UTF-16 code unit sequence");
        } else {
            cp = rcu1;
            c.set_code_point(cp);
        }
    }
};


} // namespace text
} // namespace experimental
} // namespace std


#endif // } STDTEXT_CODECS_UTF16_CODEC_HPP
