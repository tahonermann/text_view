// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#if !defined(TEXT_VIEW_CODECS_UTF32LE_CODEC_HPP) // {
#define TEXT_VIEW_CODECS_UTF32LE_CODEC_HPP


#include <climits>
#include <cstdint>
#include <text_view_detail/codecs/codec_util.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/error_status.hpp>
#include <text_view_detail/character.hpp>
#include <text_view_detail/trivial_encoding_state.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


template<Character CT, CodeUnit CUT>
class utf32le_codec {
public:
    using state_type = trivial_encoding_state;
    using state_transition_type = trivial_encoding_state_transition;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 4;
    static constexpr int max_code_units = 4;

    static_assert(sizeof(code_unit_type) * CHAR_BIT >= 8);

    template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static encode_status encode_state_transition(
        state_type &state,
        CUIT &out,
        const state_transition_type &stt,
        int &encoded_code_units)
    noexcept
    {
        encoded_code_units = 0;

        return encode_status::no_error;
    }

    template<CodeUnitOutputIterator<code_unit_type> CUIT>
    static encode_status encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    noexcept(text_detail::NoExceptOutputIterator<CUIT, code_unit_type>())
    {
        encoded_code_units = 0;

        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;
        code_point_type cp{c.get_code_point()};

        code_unit_type octet1 = (cp >>  0) & 0xFF;
        code_unit_type octet2 = (cp >>  8) & 0xFF;
        code_unit_type octet3 = (cp >> 16) & 0xFF;
        code_unit_type octet4 = (cp >> 24) & 0xFF;

        *out++ = octet1;
        ++encoded_code_units;
        *out++ = octet2;
        ++encoded_code_units;
        *out++ = octet3;
        ++encoded_code_units;
        *out++ = octet4;
        ++encoded_code_units;

        return encode_status::no_error;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::ForwardIterator<CUIT>
          && ranges::ConvertibleTo<ranges::value_type_t<CUIT>, code_unit_type>
          && ranges::Sentinel<CUST, CUIT>
    static decode_status decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        decoded_code_units = 0;

        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;
        code_point_type cp;

        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type octet1 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type octet2 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type octet3 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type octet4 = *in_next++;
        ++decoded_code_units;

        cp = ((octet1 & 0xFF) <<  0)
           | ((octet2 & 0xFF) <<  8)
           | ((octet3 & 0xFF) << 16)
           | ((octet4 & 0xFF) << 24);
        c.set_code_point(cp);

        return decode_status::no_error;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::ForwardIterator<CUIT>
          && ranges::ConvertibleTo<ranges::value_type_t<CUIT>, code_unit_type>
          && ranges::Sentinel<CUST, CUIT>
    static decode_status rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        decoded_code_units = 0;

        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;
        code_point_type cp;

        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type roctet1 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type roctet2 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type roctet3 = *in_next++;
        ++decoded_code_units;
        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type roctet4 = *in_next++;
        ++decoded_code_units;

        cp = ((roctet4 & 0xFF) <<  0)
           | ((roctet3 & 0xFF) <<  8)
           | ((roctet2 & 0xFF) << 16)
           | ((roctet1 & 0xFF) << 24);
        c.set_code_point(cp);

        return decode_status::no_error;
    }
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CODECS_UTF32LE_CODEC_HPP
