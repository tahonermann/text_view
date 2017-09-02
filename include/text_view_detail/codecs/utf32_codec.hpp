// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#if !defined(TEXT_VIEW_CODECS_UTF32_CODEC_HPP) // {
#define TEXT_VIEW_CODECS_UTF32_CODEC_HPP


#include <climits>
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
class utf32_codec {
public:
    using state_type = trivial_encoding_state;
    using state_transition_type = trivial_encoding_state_transition;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 1;
    static constexpr int max_code_units = 1;

    static_assert(sizeof(code_unit_type) * CHAR_BIT >= 32);

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

        if (cp <= 0xD7FF) {
            *out++ = code_unit_type(cp);
            ++encoded_code_units;
        } else if (cp <= 0xDFFF) {
            return encode_status::invalid_character;
        } else if (cp <= 0x0010FFFF) {
            *out++ = code_unit_type(cp);
            ++encoded_code_units;
        } else {
            return encode_status::invalid_character;
        }

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

        if (in_next == in_end)
            return decode_status::underflow;

        code_unit_type cu = *in_next;
        if ((cu >= 0x0000D800 && cu <= 0x0000DFFF) ||
            cu > 0x0010FFFF)
        {
            skip_to_valid_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        code_point_type cp(cu);
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

        if (in_next == in_end)
            return decode_status::underflow;
        code_unit_type rcu = *in_next;
        if ((rcu >= 0x0000D800 && rcu <= 0x0000DFFF) ||
            rcu > 0x0010FFFF)
        {
            skip_to_valid_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        code_point_type cp(rcu);
        c.set_code_point(cp);
        return decode_status::no_error;
    }

private:
    template<CodeUnitIterator CUIT, ranges::Sentinel<CUIT> CUST>
    static void skip_to_valid_code_unit(
        CUIT &in_next,
        CUST in_end,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        while (in_next != in_end) {
            code_unit_type cu = *in_next;
            if (cu < 0x0000D800 || (cu >= 0x0000E000 && cu <= 0x0010FFFF)) {
                // Found a valid code unit.
                return;
            }
            ++in_next;
            ++decoded_code_units;
        }
    }
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CODECS_UTF32_CODEC_HPP
