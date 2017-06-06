// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_IOS_FORMAT_PRESERVER_HPP // {
#define TEXT_VIEW_IOS_FORMAT_PRESERVER_HPP


#include <ostream>


namespace std {
namespace experimental {
inline namespace text {
namespace text_detail {


struct ios_format_preserver {
    ios_format_preserver(ostream &os)
        : os(os),
          flags(os.flags()),
          fill(os.fill())
    {}
    ~ios_format_preserver() {
        os.flags(flags);
        os.fill(fill);
    }
private:
    ostream &os;
    ios_base::fmtflags flags;
    ostream::char_type fill;
};


} // namespace text_detail
} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_IOS_FORMAT_PRESERVER_HPP
