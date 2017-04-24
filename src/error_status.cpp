// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cassert>
#include <text_view_detail/error_status.hpp>


namespace std {
namespace experimental {
inline namespace text {


const char* status_message(encode_status es) noexcept {
    switch (es) {
        case encode_status::no_error:
            return "no error";
        case encode_status::invalid_character:
            return "invalid character";
        case encode_status::invalid_state_transition:
            return "invalid state transition";
    }
    assert(0 && "Unrecognized encode_status value");
    return "unknown error";
}

const char* status_message(decode_status ds) noexcept {
    switch (ds) {
        case decode_status::no_error:
            return "no error";
        case decode_status::no_character:
            return "no character decoded";
        case decode_status::invalid_code_unit_sequence:
            return "invalid code unit sequence";
        case decode_status::underflow:
            return "underflow";
    }
    assert(0 && "Unrecognized decode_status value");
    return "unknown error";
}


} // inline namespace text
} // namespace experimental
} // namespace std
