// Copyright (c) 2016, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ITEXT_SENTINEL_HPP // {
#define TEXT_VIEW_ITEXT_SENTINEL_HPP


#include <experimental/ranges/iterator>
#include <text_view_detail/concepts.hpp>
#include <text_view_detail/error_policy.hpp>
#include <text_view_detail/itext_iterator.hpp>


namespace std {
namespace experimental {
inline namespace text {


template<
    TextEncoding ET,
    ranges::View VT,
    TextErrorPolicy TEP = text_default_error_policy>
class itext_sentinel {
public:
    using view_type = VT;
    using error_policy = TEP;
    using sentinel = ranges::sentinel_t<std::add_const_t<VT>>;

    itext_sentinel() = default;

    itext_sentinel(sentinel s)
        : s(std::move(s)) {}

    friend bool operator==(
        const itext_iterator<ET, VT, TEP> &ti,
        const itext_sentinel &ts)
    {
        return ts.equal(ti);
    }
    friend bool operator!=(
        const itext_iterator<ET, VT, TEP> &ti,
        const itext_sentinel &ts)
    {
        return !(ti == ts);
    }
    friend bool operator==(
        const itext_sentinel &ts,
        const itext_iterator<ET, VT, TEP> &ti)
    {
        return ti == ts;
    }
    friend bool operator!=(
        const itext_sentinel &ts,
        const itext_iterator<ET, VT, TEP> &ti)
    {
        return !(ts == ti);
    }

    const sentinel& base() const noexcept {
        return s;
    }

private:
    bool equal(const itext_iterator<ET, VT, TEP> &ti) const {
        // For input iterators, the base iterator corresponds to the next input
        // to be decoded.  Naively checking for base comparison only therefore
        // results in premature matches when the last code point in the input
        // is consumed.  For this reason, base equality is only considered a
        // sentinel match if the iterator is not ok.  It is assumed that the
        // reason the base iterator is not ok is that an attempt was made to
        // decode a code point after the last one in the input.  This heuristic
        // can be defeated when an iterator is not ok for other reasons.
        return ti.base() == base()
            && ! ti.is_ok();
    }
    bool equal(const itext_iterator<ET, VT, TEP> &ti) const
        requires ranges::ForwardIterator<decltype(ti.base())>
    {
        return ti.base() == base();
    }

private:
    sentinel s;
};


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ITEXT_SENTINEL_HPP
