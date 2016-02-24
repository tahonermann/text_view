// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_RANGE_BASED_FOR_HPP // {
#define TEXT_VIEW_RANGE_BASED_FOR_HPP


#include <text_view_detail/adl_customization.hpp>


// The RANGE_BASED_FOR macro provides C++11-like range-based-for behavior for
// range types that have different begin and end types.  This implementation
// is derived from Eric Niebler's RANGES_FOR macro definition in his ranges-v3
// library.
//
// FIXME: This implementation will no longer be needed if P0184R0 is adopted.
#define RANGE_BASED_FOR(element_decl, ...)                                     \
    if (bool _done = false) {}                                                 \
    else                                                                       \
     for (auto &&_range = (__VA_ARGS__);                                       \
          !_done;)                                                             \
      for (auto _begin = std::experimental::text_detail::adl_begin(_range);    \
           !_done;)                                                            \
       for (auto _end = std::experimental::text_detail::adl_end(_range);       \
            !_done;                                                            \
            _done = true)                                                      \
        for (;                                                                 \
             !_done && _begin != _end;                                         \
             ++_begin)                                                         \
         if (!(_done = true)) {}                                               \
         else                                                                  \
          for (element_decl = *_begin; _done; _done = false)


#endif // } TEXT_VIEW_RANGE_BASED_FOR_HPP
