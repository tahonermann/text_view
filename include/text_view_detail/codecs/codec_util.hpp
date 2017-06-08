// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_CODECS_CODEC_UTIL_HPP // {
#define TEXT_VIEW_CODECS_CODEC_UTIL_HPP


#include <utility>
#include <experimental/ranges/concepts>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


/*
 * No exception input iterator concept.  Used when defining exception
 * specifications that are conditional on certain input iterator/sentinel
 * operations being non-throwing.
 */
template<typename I, typename S>
concept bool NoExceptInputIterator() {
    return ranges::InputIterator<I>
        && ranges::Sentinel<S, I>
        && requires(I &i, S &s) {
               { *i } noexcept;
               { ++i } noexcept;
               { i++ } noexcept;
               { i == s } noexcept;
               { i != s } noexcept;
           };
}


/*
 * No exception output iterator concept.  Used when defining exception
 * specifications that are conditional on certain output iterator
 * operations being non-throwing.
 */
template<typename I, typename V>
concept bool NoExceptOutputIterator() {
    return ranges::OutputIterator<I, V>
        && requires(I &i, V &&v) {
               { *i = std::forward<V>(v) } noexcept;
               { *i++ = std::forward<V>(v) } noexcept;
               { ++i } noexcept;
               { i++ } noexcept;
           };
}


/*
 * Utility class to perform an increment operation at scope exit.  Used to
 * ensure the increment is performed even if an exception is thrown.
 */
template<ranges::Integral T>
class delayed_increment {
public:
    delayed_increment(T &v, T &distance) noexcept
        : v(v), distance(distance) {}
    ~delayed_increment() {
        v += distance;
    }

private:
    T &v;
    T &distance;
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_CODECS_CODEC_UTIL_HPP
