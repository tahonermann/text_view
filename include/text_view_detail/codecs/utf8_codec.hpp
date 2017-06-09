// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#if !defined(TEXT_VIEW_CODECS_UTF8_CODEC_HPP) // {
#define TEXT_VIEW_CODECS_UTF8_CODEC_HPP


#include <climits>
#include <text_view_detail/codecs/codec_util.hpp>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/character.hpp>
#include <text_view_detail/error_status.hpp>
#include <text_view_detail/trivial_encoding_state.hpp>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


template<Character CT, CodeUnit CUT>
class utf8_codec {
public:
    using state_type = trivial_encoding_state;
    using state_transition_type = trivial_encoding_state_transition;
    using character_type = CT;
    using code_unit_type = CUT;
    using unsigned_code_unit_type = std::make_unsigned_t<code_unit_type>;
    static constexpr int min_code_units = 1;
    static constexpr int max_code_units = 4;

    static_assert(sizeof(code_unit_type) * CHAR_BIT >= 8);

    template<CodeUnitOutputIterator<unsigned_code_unit_type> CUIT>
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

    template<CodeUnitOutputIterator<unsigned_code_unit_type> CUIT>
    static encode_status encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    noexcept(text_detail::NoExceptOutputIterator<CUIT, unsigned_code_unit_type>())
    {
        encoded_code_units = 0;

        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;
        code_point_type cp{c.get_code_point()};

        if (cp <= 0x0000007F) {
            *out++ = unsigned_code_unit_type(cp);
            ++encoded_code_units;
        } else if (cp <= 0x000007FF) {
            *out++ = unsigned_code_unit_type(0xC0 + ((cp >> 6) & 0x1F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + (cp & 0x3F));
            ++encoded_code_units;
        } else if (cp <= 0x0000D7FF) {
            *out++ = unsigned_code_unit_type(0xE0 + ((cp >> 12) & 0x0F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + ((cp >> 6) & 0x3F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + (cp & 0x3F));
            ++encoded_code_units;
        } else if (cp <= 0x0000DFFF) {
            return encode_status::invalid_character;
        } else if (cp <= 0x0000FFFF) {
            *out++ = unsigned_code_unit_type(0xE0 + ((cp >> 12) & 0x0F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + ((cp >> 6) & 0x3F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + (cp & 0x3F));
            ++encoded_code_units;
        } else if (cp <= 0x0010FFFF) {
            *out++ = unsigned_code_unit_type(0xF0 + ((cp >> 18) & 0x07));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + ((cp >> 12) & 0x3F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + ((cp >> 6) & 0x3F));
            ++encoded_code_units;
            *out++ = unsigned_code_unit_type(0x80 + (cp & 0x3F));
            ++encoded_code_units;
        } else {
            return encode_status::invalid_character;
        }

        return encode_status::no_error;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::ForwardIterator<CUIT>
          && ranges::ConvertibleTo<
                 ranges::value_type_t<CUIT>,
                 unsigned_code_unit_type>
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
        unsigned_code_unit_type cu1 = *in_next;
        if (is_invalid_leading_code_unit(cu1)) {
            // First code unit is not a leading code unit.
            skip_to_leading_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if (cu1 <= 0x7F) {
            cp = (cu1 & 0x7F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        if (in_next == in_end)
            return decode_status::underflow;
        unsigned_code_unit_type cu2 = *in_next;
        if (is_invalid_second_code_unit(cu1, cu2)) {
            skip_to_leading_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if ((cu1 & 0xE0) == 0xC0) {
            cp = ((cu1 & 0x1F) << 6) + (cu2 & 0x3F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        if (in_next == in_end)
            return decode_status::underflow;
        unsigned_code_unit_type cu3 = *in_next;
        if (is_invalid_third_or_fourth_code_unit(cu3)) {
            skip_to_leading_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if ((cu1 & 0xF0) == 0xE0) {
            cp = ((cu1 & 0x0F) << 12) + ((cu2 & 0x3F) << 6) + (cu3 & 0x3F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        if (in_next == in_end)
            return decode_status::underflow;
        unsigned_code_unit_type cu4 = *in_next;
        if (is_invalid_third_or_fourth_code_unit(cu4)) {
            skip_to_leading_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if ((cu1 & 0xF8) == 0xF0) {
            cp = ((cu1 & 0x07) << 18) +
                 ((cu2 & 0x3F) << 12) +
                 ((cu3 & 0x3F) << 6) +
                  (cu4 & 0x3F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        // This should be unreachable.
        skip_to_leading_code_unit(in_next, in_end, decoded_code_units);
        return decode_status::invalid_code_unit_sequence;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::ForwardIterator<CUIT>
          && ranges::ConvertibleTo<
                 ranges::value_type_t<CUIT>,
                 unsigned_code_unit_type>
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
        unsigned_code_unit_type rcu1 = *in_next;
        if (is_invalid_trailing_code_unit(rcu1)) {
            skip_to_trailing_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if (rcu1 <= 0x7F) {
            cp = (rcu1 & 0x7F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        if (in_next == in_end)
            return decode_status::underflow;
        unsigned_code_unit_type rcu2 = *in_next;
        if (rcu2 < 0x80 ||
            (rcu2 > 0xBF && ! (rcu2 >= 0xC2 && rcu2 <= 0xDF)))
        {
            skip_to_trailing_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if ((rcu2 & 0xE0) == 0xC0) {
            cp = ((rcu2 & 0x1F) << 6) + (rcu1 & 0x3F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        if (in_next == in_end)
            return decode_status::underflow;
        unsigned_code_unit_type rcu3 = *in_next;
        if (rcu3 < 0x80 ||
            (rcu3 > 0xBF &&
             ! (rcu3 >= 0xE0 &&
                rcu3 <= 0xEF &&
                ! is_invalid_second_code_unit(rcu3, rcu2))))
        {
            skip_to_trailing_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        if ((rcu3 & 0xF0) == 0xE0) {
            cp = ((rcu3 & 0x0F) << 12) + ((rcu2 & 0x3F) << 6) + (rcu1 & 0x3F);
            c.set_code_point(cp);
            return decode_status::no_error;
        }

        if (in_next == in_end)
            return decode_status::underflow;
        unsigned_code_unit_type rcu4 = *in_next;
        if (! (rcu4 >= 0xF0 &&
               rcu4 <= 0xF4 &&
               ! is_invalid_second_code_unit(rcu4, rcu3)))
        {
            skip_to_trailing_code_unit(in_next, in_end, decoded_code_units);
            return decode_status::invalid_code_unit_sequence;
        }
        ++in_next;
        ++decoded_code_units;
        cp = ((rcu4 & 0x07) << 18) +
             ((rcu3 & 0x3F) << 12) +
             ((rcu2 & 0x3F) << 6) +
              (rcu1 & 0x3F);
        c.set_code_point(cp);
        return decode_status::no_error;
    }

private:
    static bool is_invalid_leading_code_unit(
        unsigned_code_unit_type cu)
    {
        // See Unicode 9.0, table 3-7 in chapter 3.9, "Unicode Encoding Forms".
        return (cu >= 0x80 && cu <= 0xC1) ||
               (cu >= 0xF5);
    }

    static bool is_invalid_trailing_code_unit(
        unsigned_code_unit_type cu)
    {
        // See Unicode 9.0, table 3-7 in chapter 3.9, "Unicode Encoding Forms".
        return cu > 0xBF;
    }

    static bool is_invalid_second_code_unit(
        unsigned_code_unit_type cu1,
        unsigned_code_unit_type cu2)
    {
        // See Unicode 9.0, table 3-7 in chapter 3.9, "Unicode Encoding Forms".
        return (cu2 < 0x80 || cu2 > 0xBF) ||
               (cu1 == 0xE0 && cu2 < 0xA0) ||
               (cu1 == 0xED && cu2 > 0x9F) ||
               (cu1 == 0xF0 && cu2 < 0x90) ||
               (cu1 == 0xF4 && cu2 > 0x8F);
    }

    static bool is_invalid_third_or_fourth_code_unit(
        unsigned_code_unit_type cu)
    {
        // See Unicode 9.0, table 3-7 in chapter 3.9, "Unicode Encoding Forms".
        return cu < 0x80 || cu > 0xBF;
    }

    template<CodeUnitIterator CUIT, ranges::Sentinel<CUIT> CUST>
    static void skip_to_leading_code_unit(
        CUIT &in_next,
        CUST in_end,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        while (in_next != in_end) {
            unsigned_code_unit_type cu = *in_next;
            if (cu <= 0x7F || (cu >= 0xC2 && cu <= 0xF4)) {
                // Found a leading code unit.
                return;
            }
            ++in_next;
            ++decoded_code_units;
        }
    }

    template<CodeUnitIterator CUIT, ranges::Sentinel<CUIT> CUST>
    static void skip_to_trailing_code_unit(
        CUIT &in_next,
        CUST in_end,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        while (in_next != in_end) {
            unsigned_code_unit_type cu = *in_next;
            if (cu <= 0xBF) {
                // Found a trailing code unit.
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


#endif // } TEXT_VIEW_CODECS_UTF8_CODEC_HPP
