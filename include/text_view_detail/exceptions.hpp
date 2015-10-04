#if !defined(TEXT_VIEW_EXCEPTIONS_HPP) // {
#define TEXT_VIEW_EXCEPTIONS_HPP


#include <stdexcept>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Text runtime error
 */
class text_runtime_error
    : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};


/*
 * Text encode error
 */
class text_encode_error
    : public text_runtime_error
{
public:
    using text_runtime_error::text_runtime_error;
};


/*
 * Text decode error
 */
class text_decode_error
    : public text_runtime_error
{
public:
    using text_runtime_error::text_runtime_error;
};


/*
 * Text encode underflow error
 */
class text_encode_overflow_error
    : public text_runtime_error
{
public:
    using text_runtime_error::text_runtime_error;
};


/*
 * Text decode underflow error
 */
class text_decode_underflow_error
    : public text_runtime_error
{
public:
    using text_runtime_error::text_runtime_error;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_EXCEPTIONS_HPP
