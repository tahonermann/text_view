// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CODECS_TRIVIAL_CODEC_HPP // {
#define TEXT_VIEW_CODECS_TRIVIAL_CODEC_HPP


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
class trivial_codec {
public:
    using state_type = trivial_encoding_state;
    using state_transition_type = trivial_encoding_state_transition;
    using character_type = CT;
    using code_unit_type = CUT;
    static constexpr int min_code_units = 1;
    static constexpr int max_code_units = 1;

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
        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;

        code_point_type cp{c.get_code_point()};
        code_unit_type cu(cp);
        *out++ = cu;
        encoded_code_units = 1;

        return encode_status::no_error;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::ForwardIterator<CUIT>()
          && ranges::ConvertibleTo<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static decode_status decode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;

        if (in_next == in_end) {
            return decode_status::underflow;
        } else {
            code_unit_type cu{*in_next++};
            code_point_type cp(cu);
            c.set_code_point(cp);
            decoded_code_units = 1;
        }

        return decode_status::no_error;
    }

    template<CodeUnitIterator CUIT, typename CUST>
    requires ranges::ForwardIterator<CUIT>()
          && ranges::ConvertibleTo<ranges::value_type_t<CUIT>, code_unit_type>()
          && ranges::Sentinel<CUST, CUIT>()
    static decode_status rdecode(
        state_type &state,
        CUIT &in_next,
        CUST in_end,
        character_type &c,
        int &decoded_code_units)
    noexcept(text_detail::NoExceptInputIterator<CUIT, CUST>())
    {
        using code_point_type =
            code_point_type_t<character_set_type_t<character_type>>;

        if (in_next == in_end) {
            return decode_status::underflow;
        } else {
            code_unit_type cu{*in_next++};
            code_point_type cp(cu);
            c.set_code_point(cp);
            decoded_code_units = 1;
        }

        return decode_status::no_error;
    }
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CODECS_TRIVIAL_CODEC_HPP
