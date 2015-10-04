#ifndef TEXT_VIEW_BASIC_CHARSETS_HPP // {
#define TEXT_VIEW_BASIC_CHARSETS_HPP


namespace std {
namespace experimental {
inline namespace text {


/*
 * C++ basic execution character set
 * ISO/IEC 14882:2011(E) 2.3
 */
struct basic_execution_character_set {
    using code_point_type = char;
};


/*
 * C++ basic execution wide character set
 * ISO/IEC 14882:2011(E) 2.3
 */
struct basic_execution_wide_character_set {
    using code_point_type = wchar_t;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_BASIC_CHARSETS_HPP
