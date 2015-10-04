#if !defined(TEXT_VIEW_EXCEPTIONS_HPP) // {
#define TEXT_VIEW_EXCEPTIONS_HPP


#include <stdexcept>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Text runtime error
 */
struct text_runtime_error
    : public std::runtime_error
{
    explicit text_runtime_error(const std::string& what_arg)
        : std::runtime_error(what_arg) {}
    explicit text_runtime_error(const char* what_arg)
        : std::runtime_error(what_arg) {}
};


/*
 * Text encode error
 */
struct text_encode_error
    : public text_runtime_error
{
    explicit text_encode_error(const std::string& what_arg)
        : text_runtime_error(what_arg) {}
    explicit text_encode_error(const char* what_arg)
        : text_runtime_error(what_arg) {}
};


/*
 * Text decode error
 */
struct text_decode_error
    : public text_runtime_error
{
    explicit text_decode_error(const std::string& what_arg)
        : text_runtime_error(what_arg) {}
    explicit text_decode_error(const char* what_arg)
        : text_runtime_error(what_arg) {}
};


/*
 * Text encode underflow error
 */
struct text_encode_overflow_error
    : public text_runtime_error
{
    explicit text_encode_overflow_error(const std::string& what_arg)
        : text_runtime_error(what_arg) {}
    explicit text_encode_overflow_error(const char* what_arg)
        : text_runtime_error(what_arg) {}
};


/*
 * Text decode underflow error
 */
struct text_decode_underflow_error
    : public text_runtime_error
{
    explicit text_decode_underflow_error(const std::string& what_arg)
        : text_runtime_error(what_arg) {}
    explicit text_decode_underflow_error(const char* what_arg)
        : text_runtime_error(what_arg) {}
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_EXCEPTIONS_HPP
