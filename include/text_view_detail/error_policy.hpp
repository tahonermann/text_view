// Copyright (c) 2017, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#ifndef TEXT_VIEW_ERROR_POLICY_HPP // {
#define TEXT_VIEW_ERROR_POLICY_HPP


namespace std {
namespace experimental {
inline namespace text {


#if !defined(TEXT_VIEW_DEFAULT_ERROR_POLICY)
#define TEXT_VIEW_DEFAULT_ERROR_POLICY \
        ::std::experimental::text_strict_error_policy
#endif


// Base class for text error policies.  This is used to unambiguously identify
// error policy classes.
class text_error_policy {};

// When the strict error policy is in effect, exceptions are thrown for any
// encoding or decoding errors that are detected.
class text_strict_error_policy : public text_error_policy {};

// When the permissive error policy is in effect, errors detected during
// encoding or decoding are tolerated without throwing exceptions.  Errors
// detected while encoding a character result in encoding the code unit
// sequence for the replacement character defined by the encoding.  Errors
// detected while encoding state transitions may result in the state
// transition request being discarded, the state being reset to a default
// state, or a substitution character being encoded.  Errors detected while
// decoding result in code units being consumed until a resynchronization
// point or the end of the code unit sequence is reached, and the replacement
// character defined by the encoding being provided as if it had been decoded.
// For Unicode encoding forms, resyncronization points are determined based on
// the "maximal subpart of an ill-formed subsequence" as defined in section
// 3.9 of the Unicode 9.0 standard.
class text_permissive_error_policy : public text_error_policy {};


using text_default_error_policy = TEXT_VIEW_DEFAULT_ERROR_POLICY;


} // inline namespace text
} // namespace experimental
} // namespace std


#endif // } TEXT_VIEW_ERROR_POLICY_HPP
