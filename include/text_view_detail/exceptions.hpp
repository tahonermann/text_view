// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#if !defined(TEXT_VIEW_EXCEPTIONS_HPP) // {
#define TEXT_VIEW_EXCEPTIONS_HPP


#include <text_view_detail/error_status.hpp>
#include <stdexcept>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Text runtime error
 */
class text_error
    : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};


/*
 * Text encode error
 */
class text_encode_error
    : public text_error
{
public:
    explicit text_encode_error(encode_status es) noexcept
        : text_error(status_message(es)),
          es(es)
        {}

    const encode_status& status_code() const noexcept {
        return es;
    }

private:
    encode_status es;
};


/*
 * Text decode error
 */
class text_decode_error
    : public text_error
{
public:
    explicit text_decode_error(decode_status ds) noexcept
        : text_error(status_message(ds)),
          ds(ds)
        {}

    const decode_status& status_code() const noexcept {
        return ds;
    }

private:
    decode_status ds;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_EXCEPTIONS_HPP
