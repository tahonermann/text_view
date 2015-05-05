#ifndef TEXT_VIEW_ANY_CHARSET_HPP // {
#define TEXT_VIEW_ANY_CHARSET_HPP


#include <cstdint>


namespace std {
namespace experimental {
namespace text_view {


/*
 * Any character set
 * This character set is associated with characters that do not have a
 * statically known character set.
 */
struct any_character_set {
    using code_point_type = uint_least32_t;
};


} // namespace text_view
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ANY_CHARSET_HPP
