#ifndef TEXT_VIEW_ANY_CHARSET_HPP // {
#define TEXT_VIEW_ANY_CHARSET_HPP


#include <cstdint>


namespace std {
namespace experimental {
inline namespace text {


/*
 * Any character set
 * This character set is associated with characters that do not have a
 * statically known character set.
 */
struct any_character_set {
    using code_point_type = uint_least32_t;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ANY_CHARSET_HPP
