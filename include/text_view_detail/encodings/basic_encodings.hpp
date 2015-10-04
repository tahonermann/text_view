#ifndef TEXT_VIEW_BASIC_ENCODINGS_HPP // {
#define TEXT_VIEW_BASIC_ENCODINGS_HPP


#include <text_view_detail/charsets/basic_charsets.hpp>
#include <text_view_detail/character.hpp>
#include <text_view_detail/codecs/trivial_codec.hpp>


namespace std {
namespace experimental {
inline namespace text {


/*
 * C++ basic execution character encoding
 */
struct basic_execution_character_encoding {
    using codec_type = trivial_codec<
              character<basic_execution_character_set>,
              char>;
    using state_type = codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};


/*
 * C++ basic execution wide character encoding
 */
struct basic_execution_wide_character_encoding {
    using codec_type = trivial_codec<
              character<basic_execution_wide_character_set>,
              wchar_t>;
    using state_type = codec_type::state_type;
    static const state_type& initial_state() noexcept {
        static const state_type state;
        return state;
    }
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_BASIC_ENCODINGS_HPP
