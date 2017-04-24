// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ERROR_STATUS_HPP // {
#define TEXT_VIEW_ERROR_STATUS_HPP


namespace std {
namespace experimental {
inline namespace text {


enum class encode_status : int {
    // Indicates that no error has occurred.
    no_error = 0,
    // Indicates that an attempt was made to encode an invalid character.
    invalid_character = -1,
    // Indicates that an attempt was made to encode an invalid state transition.
    invalid_state_transition = -2
};

enum class decode_status : int {
    // Indicates that no error has occurred.
    no_error = 0,
    // Indicates that no error has occurred, but that no character was decoded
    // for a code unit sequence.  This typically indicates that the code unit
    // sequence represents an encoding state transition such as for an escape
    // sequence or byte order marker.
    no_character = 1,
    // Indicates that an attempt was made to decode an invalid code unit
    // sequence.
    invalid_code_unit_sequence = -1,
    // Indicates that the end of the input range was encountered before a
    // complete code unit sequence was decoded.
    underflow = -2
};


constexpr inline bool status_ok(encode_status es) noexcept {
    return static_cast<int>(es) >= 0;
}
constexpr inline bool status_ok(decode_status ds) noexcept {
    return static_cast<int>(ds) >= 0;
}

constexpr inline bool error_occurred(encode_status es) noexcept {
    return static_cast<int>(es) < 0;
}
constexpr inline bool error_occurred(decode_status ds) noexcept {
    return static_cast<int>(ds) < 0;
}


const char* status_message(encode_status es) noexcept;
const char* status_message(decode_status ds) noexcept;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ERROR_STATUS_HPP
