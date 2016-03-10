// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#if !defined(TEXT_VIEW_CODECS_UTF8BOM_CODEC_HPP) // {
#define TEXT_VIEW_CODECS_UTF8BOM_CODEC_HPP


#include <cassert>
#include <climits>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/codecs/utf8_codec.hpp>


namespace std {
namespace experimental {
inline namespace text {


/*
 *  to_initial
 *  +-----+
 *  |     v
 *  |  +---------------+  to_bom_written   /---------\
 *  |  | initial state |>---------------->| write BOM |
 *  |  +---------------+                   \---------/
 *  |     v    v                                v
 *  |     |    |to_assume_bom_written           |e
 *  ^-----+    |                                |
 *  |          |          +-----<---------------+
 *  |          v          v     |
 *  |  +---------------------+  |to_bom_written
 *  |  | BOM read or written |  |to_assume_bom_written
 *  |  +---------------------+  |
 *  |     v               v     |
 *  +-----+               +-----+
 */
struct utf8bom_encoding_state_transition {
    enum {
        to_initial,
        to_bom_written,
        to_assume_bom_written
    } state_transition;

    static utf8bom_encoding_state_transition
    to_initial_state() noexcept {
        return { to_initial };
    }

    static utf8bom_encoding_state_transition
    to_bom_written_state() noexcept {
        return { to_bom_written };
    }

    static utf8bom_encoding_state_transition
    to_assume_bom_written_state() noexcept {
        return { to_assume_bom_written };
    }
};

struct utf8bom_encoding_state {
    bool bom_read_or_written : 1;
};


namespace text_detail {

template<Character CT, CodeUnit CUT>
class utf8bom_codec {
public:
    using state_type = utf8bom_encoding_state;
    using state_transition_type = utf8bom_encoding_state_transition;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 1;
    static constexpr int max_code_units = 4;

    static_assert(sizeof(code_unit_type) * CHAR_BIT >= 8);

    template<CodeUnitOutputIterator<std::make_unsigned_t<code_unit_type>> CUIT>
    static void encode_state_transition(
        state_type &state,
        CUIT &out,
        const state_transition_type &stt,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        using unsigned_code_unit_type =
            std::make_unsigned_t<code_unit_type>;

        switch (stt.state_transition) {
            case state_transition_type::to_initial:
                state.bom_read_or_written = false;
                break;
            case state_transition_type::to_bom_written:
                if (! state.bom_read_or_written) {
                    *out++ = unsigned_code_unit_type(0xEF);
                    ++encoded_code_units;
                    *out++ = unsigned_code_unit_type(0xBB);
                    ++encoded_code_units;
                    *out++ = unsigned_code_unit_type(0xBF);
                    ++encoded_code_units;
                    state.bom_read_or_written = true;
                }
                break;
            case state_transition_type::to_assume_bom_written:
                state.bom_read_or_written = true;
                break;
        }
    }

    template<CodeUnitOutputIterator<std::make_unsigned_t<code_unit_type>> CUIT>
    static void encode(
        state_type &state,
        CUIT &out,
        character_type c,
        int &encoded_code_units)
    {
        encoded_code_units = 0;

        if (! state.bom_read_or_written) {
            encode_state_transition(
                state, out, state_transition_type::to_bom_written_state(),
                encoded_code_units);
        }

        using utf8_codec = utf8_codec<CT, CUT>;
        using utf8_state_type = typename utf8_codec::state_type;
        static_assert(std::is_empty<utf8_state_type>::value);

        utf8_state_type utf8_state;
        int utf8_encoded_code_units = 0;
        try {
            utf8_codec::encode(utf8_state, out, c, utf8_encoded_code_units);
        } catch(...) {
            encoded_code_units += utf8_encoded_code_units;
            throw;
        }
        encoded_code_units += utf8_encoded_code_units;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::ConvertibleTo<
                 ranges::value_type_t<CUIT>,
                 std::make_unsigned_t<code_unit_type>>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        using utf8_codec = utf8_codec<CT, CUT>;
        using utf8_state_type = typename utf8_codec::state_type;
        static_assert(std::is_empty<utf8_state_type>::value);

        utf8_state_type utf8_state;
        int utf8_decoded_code_units = 0;
        bool return_value;
        try {
            return_value = utf8_codec::decode(utf8_state, in_next, in_end, c,
                                              utf8_decoded_code_units);
        } catch(...) {
            decoded_code_units += utf8_decoded_code_units;
            throw;
        }
        decoded_code_units += utf8_decoded_code_units;

        assert(return_value);
        if (! state.bom_read_or_written
            && c.get_code_point() == 0xFEFF)
        {
            // A BOM has been read at the start of input.  Adjust the state
            // and return false to indicate that a code point has not been
            // decoded.
            return_value = false;
        }
        state.bom_read_or_written = true;

        return return_value;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::InputIterator<CUIT>()
          && ranges::ConvertibleTo<
                 ranges::value_type_t<CUIT>,
                 std::make_unsigned_t<code_unit_type>>()
          && ranges::Sentinel<CUST, CUIT>()
    static bool rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    {
        decoded_code_units = 0;

        using utf8_codec = utf8_codec<CT, CUT>;
        using utf8_state_type = typename utf8_codec::state_type;
        static_assert(std::is_empty<utf8_state_type>::value);

        utf8_state_type utf8_state;
        int utf8_decoded_code_units = 0;
        bool return_value;
        try {
            return_value = utf8_codec::rdecode(utf8_state, in_next, in_end, c,
                                               utf8_decoded_code_units);
        } catch(...) {
            decoded_code_units += utf8_decoded_code_units;
            throw;
        }
        decoded_code_units += utf8_decoded_code_units;

        assert(return_value);
        if (in_next == in_end) {
            state.bom_read_or_written = false;
            if (c.get_code_point() == 0xFEFF) {
                // A BOM has been read at the start of input.  Return false to
                // indicate that a code point has not been decoded.
                return_value = false;
            }
        }

        return return_value;
    }
};

} // namespace text_detail


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CODECS_UTF8BOM_CODEC_HPP
