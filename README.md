# [Text_view]
A [C++ Concepts][ISO/IEC 19217:2015]
based character encoding and code point enumeration library.

- [Overview](#overview)
- [Requirements](#requirements)
- [Build and installation](#build-and-installation)
  - [Building and installing gcc]
    (#building-and-installing-gcc)
  - [Building and installing Origin-text_view]
    (#building-and-installing-origin-text_view)
  - [Building and installing Text_view]
    (#building-and-installing-text_view)
- [Usage](#usage)
  - [Header &lt;text_view&gt; synopsis](#header-text_view-synopsis)
  - [Concept definitions](#concept-definitions)
    - [Concept Code_unit](#concept-code_unit)
    - [Concept Code_point](#concept-code_point)
    - [Concept Character_set](#concept-character_set)
    - [Concept Character](#concept-character)
    - [Concept Code_unit_iterator](#concept-code_unit_iterator)
    - [Concept Text_encoding_state](#concept-text_encoding_state)
    - [Concept Text_encoding_state_transition]
      (#concept-text_encoding_state_transition)
    - [Concept Text_encoding](#concept-text_encoding)
    - [Concept Text_encoder](#concept-text_encoder)
    - [Concept Text_decoder](#concept-text_decoder)
    - [Concept Text_forward_decoder](#concept-text_forward_decoder)
    - [Concept Text_bidirectional_decoder](#concept-text_bidirectional_decoder)
    - [Concept Text_random_access_decoder](#concept-text_random_access_decoder)
    - [Concept Text_iterator](#concept-text_iterator)
    - [Concept Text_sentinel](#concept-text_sentinel)
    - [Concept Text_view](#concept-text_view)
- [Supported Encodings](#supported-encodings)
- [Terminology](#terminology)
  - [Code Unit](#code-unit)
  - [Code Point](#code-point)
  - [Character Set](#character-set)
  - [Character](#character)
  - [Encoding](#encoding)
- [References](#references)

# Overview
[C++11][ISO/IEC 14882:2011] added support for new character types ([N2249]) and
[Unicode] string literals ([N2442]), but neither [C++11][ISO/IEC 14882:2011],
nor more recent standards have provided means of efficiently and conveniently
enumerating [code points](#code-point) in [Unicode] or legacy encodings.  While
it is possible to implement such enumeration using interfaces provided in the
standard `<codecvt>` library, doing to is awkward, requires that text be
provided as pointers to contiguous memory, and ineffient due to virtual function
call overhead (__examples and data required to back up these assertions__).

[Text_view] provides iterator and range based interfaces for encoding and
decoding strings in a variety of [character encodings](#encoding).  The
interface is intended to support all modern and legacy
[character encodings](#encoding), though this library does not yet provide
implementations for legacy [encodings](#encoding).

An example usage follows.  Note that `\u00F8` (LATIN SMALL LETTER O WITH STROKE)
is encoded as UTF-8 using two [code units](#code-unit) (`\xC3\xB8`), but
iterator based enumeration sees just the single [code point](#code-point).

```C++
using CT = utf8_encoding::character_type;
auto tv = make_text_view<utf8_encoding>(u8"J\u00F8erg is my friend");
auto it = tv.begin();
assert(*it++ == CT{0x004A}); // 'J'
assert(*it++ == CT{0x00F8}); // 'ø'
assert(*it++ == CT{0x0065}); // 'e'
```

The iterators and ranges that [Text_view] provides are compatible with the
non-modifying sequence utilities provided by the standard C++ `<algorithm>`
library.  This enables use of standard algorithms to search encoded text.

```C++
it = std::find(tv.begin(), tv.end(), CT{0x00F8});
assert(it != tv.end());
```

The iterators provided by [Text_view] also provide access to the underlying
[code unit](#code-unit) sequence.

```C++
auto base_it = it.base_range().begin();
assert(*base_it++ == '\xC3');
assert(*base_it++ == '\xB8');
```

[Text_view] ranges satisfy the requirements for use in range-based for
statements.

```C++
for (const auto &ch : tv) {
  ...
}
```

# Requirements
[Text_view] requires a C++ compiler that implements [ISO/IEC technical
specification 19217:2015, **C++ Extensions for concepts**][ISO/IEC 19217:2015]
As of 2015-12-31, this specification is only supported by the current
in-development release of [gcc] that currently self-identifies itself as [gcc]
version 6.0.0.  Additionally, [Text_view] depends on the [Origin-text_view] fork
of [Andrew Sutton]'s [Origin] library for concept definitions.  The fork is
necessary to work around the following [gcc] defects and [Origin] issues that
are surfaced by [Text_view].

1. https://github.com/asutton/origin/issues/5
2. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=69235
3. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=67565

# Build and installation
This section provides instructions for building [Text_view] and suitable
versions of its dependencies.

## Building and installing [gcc]
[Text_view] is known to build successfully with [gcc] svn revision 232017; the
top of trunk as of 2015-12-31.  The following commands can be used to perform a
suitable build of that [gcc] revision on Linux.

```sh
$ svn co svn://gcc.gnu.org/svn/gcc/trunk gcc-trunk-src
$ curl -O ftp://ftp.gnu.org/gnu/gmp/gmp-5.1.1.tar.bz2
$ curl -O ftp://ftp.gnu.org/gnu/mpfr/mpfr-3.1.2.tar.bz2
$ curl -O ftp://ftp.gnu.org/gnu/mpc/mpc-1.0.1.tar.gz
$ cd gcc-trunk-src
$ svn update 32017  # Optional command to select a known good gcc version
$ bzip2 -d -c ../gmp-5.1.1.tar.bz2 | tar -xvf -
$ mv gmp-5.1.1 gmp
$ bzip2 -d -c ../mpfr-3.1.2.tar.bz2 | tar -xvf -
$ mv mpfr-3.1.2 mpfr
$ tar -zxvf ../mpc-1.0.1.tar.gz
$ mv mpc-1.0.1 mpc
$ cd ..
$ mkdir gcc-trunk-build
$ cd gcc-trunk-build
$ LIBRARY_PATH=/usr/lib/$(gcc -print-multiarch); export LIBRARY_PATH
$ CPATH=/usr/include/$(gcc -print-multiarch); export CPATH
$ ../gcc-trunk-src/configure \
  CC=gcc \
  CXX=g++ \
  --prefix $(pwd)/../gcc-trunk-install \
  --disable-multilib \
  --disable-bootstrap \
  --enable-languages=c,c++
$ make -j 4
$ make install
$ cd ..
```

When complete, the new [gcc] build will be present in the `gcc-trunk-install`
directory.

## Building and installing [Origin-text_view]
[Text_view] does not currently build successfully with the top of trunk version
of the [Origin] library.  The [Origin-text_view] fork of [Origin] was created
to work around the issues.  The following commands can be used to perform a
suitable build of [Origin] on Linux that will work with [Text_view].  Note that
these commands assume that an appropriate build of [gcc] was performed and
installed as per the previous section.

```sh
$ PATH=$(pwd)/gcc-trunk-install/bin; export PATH
$ git clone https://github.com/tahonermann/origin.git origin-text_view-src
$ mkdir origin-text_view-build
$ cd origin-text_view-build
$ cmake ../origin-text_view-src \
  -DCMAKE_INSTALL_PREFIX:PATH=$(pwd)/../origin-text_view-install
$ make install
$ cd ..
```

## Building and installing [Text_view]
[Text_view] is currently a header-only library, so no build or installation is
currently required to use it.  It is unlikely that [Text_view] will remain a
header-only library in the future, so at some point, a build and installation
step will be required.

[Text_view] currently has a simple build system that is hard-coded to look for
[gcc] and [Origin] at specific locations.  This will change at some point in
the future, but given the present [gcc] and [Origin] requirements, hasn't been
a burden.  The build system is only used to build and run a few test programs.

The following commands suffice to build and run the test programs.  Note that it
may take several minutes to build the test-text_view program.

```sh
$ vi setenv-gcc-trunk.sh  # Update GCC_INSTALL_PATH and ORIGIN_INSTALL_PATH.
$ . ./setenv-gcc-trunk.sh
$ make
```

If the build succeeds, a few test and utility programs will be present in the
`bin` directory.

# Usage
[Text_view] is currently a header-only library.  To use it in your own code,
add include paths for the `text_view/include` and [Origin] installation
locations, and include the `text_view` header file:

```C++
#include <text_view>
```

All interfaces intended for public use are declared in the
`std::experimental::text` namespace.  The `text` namespace is an inline
namespace, so all entities are available from the `std::experimental` namespace
itself.

## Header &lt;text_view&gt; synopsis

```C++
namespace std {
namespace experimental {
inline namespace text {

// concepts:
template<typename T> concept bool Code_unit();
template<typename T> concept bool Code_point();
template<typename T> concept bool Character_set();
template<typename T> concept bool Character();
template<typename T> concept bool Code_unit_iterator();
template<typename T> concept bool Text_encoding_state();
template<typename T> concept bool Text_encoding_state_transition();
template<typename T> concept bool Text_encoding();
template<typename T, typename I> concept bool Text_encoder();
template<typename T, typename I> concept bool Text_decoder();
template<typename T, typename I> concept bool Text_forward_decoder();
template<typename T, typename I> concept bool Text_bidirectional_decoder();
template<typename T, typename I> concept bool Text_random_access_decoder();
template<typename T> concept bool Text_iterator();
template<typename T, typename I> concept bool Text_sentinel();
template<typename T> concept bool Text_view();

// character sets:
class any_character_set;
class basic_execution_character_set;
class basic_execution_wide_character_set;
class unicode_character_set;

using execution_character_set = /* implementation-defined */ ;
using execution_wide_character_set = /* implementation-defined */ ;
using universal_character_set = /* implementation-defined */ ;

class character_set_info;
class character_set_id;

template<typename CST>
  inline const character_set_info& get_character_set_info();
const character_set_info& get_character_set_info(character_set_id id);
template<typename CST>
  inline character_set_id get_character_set_id();

template<typename T>
  using code_point_type_of = /* implementation-defined */ ;
template<typename T>
  using character_set_type_of = /* implementation-defined */ ;
template<typename T>
  using encoding_type_of = /* implementation-defined */ ;

// characters:
template<Character_set CST> class character;
template <> class character<any_character_set>;

template<Character_set CST>
  bool operator==(const character<any_character_set> &c1,
                  const character<CST> &c2);
template<Character_set CST>
  bool operator==(const character<CST> &c1,
                  const character<any_character_set> &c2);
template<Character_set CST>
  bool operator!=(const character<any_character_set> &c1,
                  const character<CST> &c2);
template<Character_set CST>
  bool operator!=(const character<CST> &c1,
                  const character<any_character_set> &c2);

// encodings:
class basic_execution_character_encoding;
class basic_execution_wide_character_encoding;
#if defined(__STDC_ISO_10646__)
class iso_10646_wide_character_encoding;
#endif // __STDC_ISO_10646__
class utf8_encoding;
class utf8bom_encoding;
class utf16_encoding;
class utf16be_encoding;
class utf16le_encoding;
class utf16bom_encoding;
class utf32_encoding;
class utf32be_encoding;
class utf32le_encoding;
class utf32bom_encoding;

using execution_character_encoding = /* implementation-defined */ ;
using execution_wide_character_encoding = /* implementation-defined */ ;
using char8_character_encoding = /* implementation-defined */ ;
using char16_character_encoding = /* implementation-defined */ ;
using char32_character_encoding = /* implementation-defined */ ;

// itext_iterator:
template<Text_encoding ET, origin::Input_range RT>
  requires Text_decoder<ET, origin::Iterator_type<const RT>>()
    class itext_iterator;

// itext_sentinel:
template<Text_encoding ET, origin::Input_range RT>
  class itext_sentinel;

// otext_iterator:
template<Text_encoding E, Code_unit_iterator CUIT>
  requires origin::Output_iterator<CUIT, typename E::code_unit_type>()
    class otext_iterator;

// basic_text_view:
template<Text_encoding ET, origin::Input_range RT>
  class basic_text_view;

// basic_text_view synonyms:
using text_view = basic_text_view<execution_character_encoding,
                                  /* implementation-defined */ >;
using wtext_view = basic_text_view<execution_wide_character_encoding,
                                   /* implementation-defined */ >;
using u8text_view = basic_text_view<char8_character_encoding,
                                    /* implementation-defined */ >;
using u16text_view = basic_text_view<char16_character_encoding,
                                     /* implementation-defined */ >;
using u32text_view = basic_text_view<char32_character_encoding,
                                     /* implementation-defined */ >;

// basic_text_view factory functions:
template<Text_encoding ET, origin::Input_iterator IT, origin::Sentinel<IT> ST>
  auto make_text_view(typename ET::state_type state, IT first, ST last);
template<Text_encoding ET, origin::Input_iterator IT, origin::Sentinel<IT> ST>
  auto make_text_view(IT first, ST last);
template<Text_encoding ET, origin::Forward_iterator IT>
  auto make_text_view(typename ET::state_type state,
                      IT first,
                      origin::Make_unsigned<origin::Difference_type<IT>> n);
template<Text_encoding ET, origin::Forward_iterator IT>
  auto make_text_view(IT first,
                      origin::Make_unsigned<origin::Difference_type<IT>> n);
template<Text_encoding ET, origin::Input_range Iterable>
  auto make_text_view(typename ET::state_type state,
                      const Iterable &iterable);
template<Text_encoding ET, origin::Input_range Iterable>
  auto make_text_view(const Iterable &iterable);
template<Text_iterator TIT, Text_sentinel<TIT> TST>
  auto make_text_view(TIT first, TST last);
template<Text_view TVT>
auto make_text_view(TVT tv);

// make_cstr_range:
template<Code_unit CUT, std::size_t N>
  auto make_cstr_range(const CUT (&cstr)[N]);

} // inline namespace text
} // namespace experimental
} // namespace std
```

## Concept definitions

### Concept Code_unit
The `Code_unit` concept specifies requirements for a type usable as the
[code unit](#code-unit) type of a string type.

```C++
template<typename T> concept bool Code_unit() {
    return /* implementation-defined */ ;
}
```

`Code_unit<T>()` is satisfied if and only if
`std::is_integral<T>::value` is true and at least one of
`std::is_unsigned<T>::value` is true,
`std::is_same<std::remove_cv<T>::type, char>::value` is true, or
`std::is_same<std::remove_cv<T>::type, wchar_t>::value` is true.

### Concept Code_point
The `Code_point` concept specifies requirements for a type usable as the
[code point](#code-point) type of a [character set](#character-set) type.

```C++
template<typename T> concept bool Code_point() {
    return /* implementation-defined */ ;
}
```

`Code_point<T>()` is satisfied if and only if
`std::is_integral<T>::value` is true and at least one of
`std::is_unsigned<T>::value` is true,
`std::is_same<std::remove_cv<T>::type, char>::value` is true, or
`std::is_same<std::remove_cv<T>::type, wchar_t>::value` is true.

### Concept Character_set
The `Character_set` concept specifies requirements for a type that describes
a [character set](#character-set).  Such a type has a member typedef-name
declaration for a type that satisfies `Code_point` and a static member function
that returns a name for the [character set](#character-set).

```C++
template<typename T> concept bool Character_set() {
  return Code_point<typename T::code_point_type>()
      && requires () {
           { T::get_name() } noexcept -> const char *;
         };
}
```

### Concept Character
The `Character` concept specifies requirements for a type that describes a
[character](#character) as defined by an associated
[character set](#character-set).  Non-static member functions provide access to
the [code point](#code-point) value of the described [character](#character).
Types that satisfy `Character` are regular and copy assignable.

```C++
template<typename T> concept bool Character() {
  return origin::Regular<T>()
      && origin::Copy_assignable<T>()
      && Character_set<typename T::character_set_type>()
      && requires (T t, typename T::character_set_type::code_point_type cp) {
           t.set_code_point(cp);
           { t.get_code_point() } -> typename T::character_set_type::code_point_type;
           { t.get_character_set_id() } -> character_set_id;
         };
}
```

### Concept Code_unit_iterator
The `Code_unit_iterator` concept specifies requirements of an iterator that
has a value type that satisfies `Code_unit`.

```C++
template<typename T> concept bool Code_unit_iterator() {
  return origin::Iterator<T>()
      && Code_unit<origin::Value_type<T>>();
}
```

### Concept Text_encoding_state

```C++
template<typename T> concept bool Text_encoding_state();
```

### Concept Text_encoding_state_transition

```C++
template<typename T> concept bool Text_encoding_state_transition();
```

### Concept Text_encoding

```C++
template<typename T> concept bool Text_encoding();
```

### Concept Text_encoder

```C++
template<typename T, typename I> concept bool Text_encoder();
```

### Concept Text_decoder

```C++
template<typename T, typename I> concept bool Text_decoder();
```

### Concept Text_forward_decoder

```C++
template<typename T, typename I> concept bool Text_forward_decoder();
```

### Concept Text_bidirectional_decoder

```C++
template<typename T, typename I> concept bool Text_bidirectional_decoder();
```

### Concept Text_random_access_decoder

```C++
template<typename T, typename I> concept bool Text_random_access_decoder();
```

### Concept Text_iterator

```C++
template<typename T> concept bool Text_iterator();
```

### Concept Text_sentinel

```C++
template<typename T, typename I> concept bool Text_sentinel();
```

### Concept Text_view

```C++
template<typename T> concept bool Text_view();
```

# Supported Encodings
As of 2015-12-31, supported [encodings](#encoding) include:

Encoding name | Description | Encoding type
------------- | ----------- | -------------
execution_character_encoding | Type alias for the encoding of ordinary string literals (C++11 2.3p3) | implementation defined
execution_wide_character_encoding | Type alias for the encoding of wide string literals (C++11 2.3p3) | implementation defined
char8_character_encoding | Type alias for the encoding of UTF-8 string literals (C++11 2.14.5p7) | implementation defined
char16_character_encoding | Type alias for the encoding of char16_t string literals (C++11 2.14.5p9) | implementation defined
char32_character_encoding | Type alias for the encoding of char32_t string literals (C++11 2.14.5p10) | implementation defined
basic_execution_character_encoding | An encoding that meets the minimum requirements of C++11 2.3p3 | trivial
basic_execution_wide_character_encoding | An encoding that meets the minimum requirements of C++11 2.3p3 | trivial
iso_10646_wide_character_encoding | An ISO 10646 encoding.  Only defined if __STDC_ISO_10646__ is defined | trivial
utf8_encoding | [Unicode] UTF-8 | stateless, variable width
utf8bom_encoding | [Unicode] UTF-8 with a byte order mark | stateful, variable width
utf16_encoding | [Unicode] UTF-16, native endian | stateless, variable width
utf16be_encoding | [Unicode] UTF-16, big endian | stateless, variable width
utf16le_encoding | [Unicode] UTF-16, little endian | stateless, variable width
utf16bom_encoding | [Unicode] UTF-16 with a byte order mark | stateful, variable width
utf32_encoding | [Unicode] UTF-32, native endian | stateless, fixed width
utf32be_encoding | [Unicode] UTF-16, big endian | stateless, fixed width
utf32le_encoding | [Unicode] UTF-16, little endian | stateless, fixed width
utf32bom_encoding | [Unicode] UTF-32 with a byte order mark | stateful, variable width

# Terminology
The terminology used in this document and in the [Text_view] library has been
chosen to be consistent with industry standards and, in particular, the
[Unicode standard].  Any inconsistencies in the use of this terminology and that
in the [Unicode standard] is unintentional.  The terms described in this
document and used within the [Text_view] library comprise a subset of the
terminology used within the [Unicode standard]; only those terms necessary to
specify functionality exhibited by this library are included here.  Those who
would like to learn more about general text processing terminology in computer
systems are encouraged to read chatper 2, "General Structure" of the
[Unicode standard].

## Code Unit
A single, indivisible, integral element of an encoded sequence of characters.  A
sequence of one or more code units specifies a code point or encoding state
transition as defined by a character encoding.  A code unit does not, by itself,
identify any particular character or code point; the meaning ascribed to a
particular code unit value is derived from a character encoding definition.

The [C++11][ISO/IEC 14882:2011] `char`, `wchar_t`, `char16_t`, and `char32_t`
types are most commonly used as code unit types.

The string literal `u8"J\u00F8erg"` contains 7 code units and 6 code unit
sequences; "\u00F8" is encoded using two code units and string literals contain
a trailing NUL code unit.

The string literal `"J\u00F8erg"` contains an implementation defined number of
code units.  The standard does not specify the encoding of ordinary and wide
string literals, so the number of code units encoded by "\u00F8" depends on the
implementation defined encoding used for ordinary string literals.

## Code Point
An integral value denoting an abstract character as defined by a character set.
A code point does not, by itself, identify any particular character; the
meaning ascribed to a particular code point value is derived from a character
set definition.

The [C++11][ISO/IEC 14882:2011] `char`, `wchar_t`, `char16_t`, and `char32_t`
types are most commonly used as code point types.

The string literal `u8"J\u00F8erg"` describes a sequence of 6 code point values;
string literals implicitly specify a trailing NUL code point.

The string literal `"J\u00F8erg"` describes a sequence of an implementation
defined number of code point values.  The standard does not specify the encoding
of ordinary and wide string literals, so the number of code points encoded by
"\u00F8" depends on the implementation defined encoding used for ordinary string
literals.  Implementations are permitted to translate a single code point in the
source or Unicode character sets to multiple code points in the execution
encoding.

## Character Set
A mapping of code point values to abstract characters.  A character set need not
provide a mapping for every possible code point value representable by the code
point type.

[C++11][ISO/IEC 14882:2011] does not specify the use of any particular character
set or encoding for ordinary and wide character and string literals, though it
does place some restrictions on them.  Unicode character and string literals are
governed by the [Unicode] standard.

Common character sets include [ASCII], [Unicode][Unicode code charts], and
[Windows code page 1252].

## Character
An element of written language, for example, a letter, number, or symbol.  A
character is identified by the combination of a character set and a code point
value.

## Encoding
A method of representing a sequence of characters as a sequence of code unit
sequences.

An encoding may be stateless or stateful.  In stateless encodings, characters
may be encoded or decoded starting from the beginning of any code unit sequence.
In stateful encodings, it may be necessary to record certain affects of
previously encoded characters in order to correctly encode additional
characters, or to decode preceding code unit sequences in order to correctly
decode following code unit sequences.

An encoding may be fixed width or variable width.  In fixed width encodings,
all characters are encoded using a single code unit sequence and all code unit
sequences have the same length.  In variable width encodings, different
characters may require multiple code unit sequences, or code unit sequences of
varying length.

An encoding may support bidirectional or random access decoding of code unit
sequences.  In bidirectional encodings, characters may be decoded by traversing
code unit sequences in reverse order.  Such encodings must support a method to
identify the start of a preceding code unit sequence.  In random access
encodings, characters may be decoded from any code unit sequence within the
sequence of code unit sequences, in constant time, without having to decode any
other code unit sequence.  Random access encodings are necessarily stateless
and fixed length.  An encoding that is neither bidirectional, nor random
access, may only be decoded by traversing code unit sequences in forward order.

An encoding may support encoding characters from multiple character sets.  Such
an encoding is either stateful and defines code unit sequences that switch the
active character set, or defines code unit sequences that implicitly identify
a character set, or both.

A trivial encoding is one in which all encoded characters correspond to a single
character set and where each code unit encodes exactly one character using the
same value as the code point for that character.  Such an encoding is stateless,
fixed width, and supports random access decoding.

Common encodings include the [Unicode] UTF-8, UTF-16, and UTF-32 encodings, the
ISO/IEC 8859 series of encodings including [ISO/IEC 8859-1], and many trivial
encodings such as [Windows code page 1252].

# References
- [Text_view]  
  https://github.com/tahonermann/text_view
- [Origin]  
  http://asutton.github.io/origin
- [Origin-text_view]  
  https://github.com/tahonermann/origin
- [ISO/IEC technical specification 19217:2015, C++ Extensions for concepts][ISO/IEC 19217:2015]  
  http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031

[Text_view]:
https://github.com/tahonermann/text_view
(Text_view library)
[Andrew Sutton]:
https://github.com/asutton
(Andrew Sutton)
[Origin]:
http://asutton.github.io/origin
(Origin libraries)
[Origin-text_view]:
https://github.com/tahonermann/origin
(Origin libraries for text_view)
[Unicode]:
http://unicode.org
(The Unicode Consortium)
[Unicode standard]:
http://www.unicode.org/standard/standard.html
(The Unicode Standard)
[Unicode code charts]:
http://www.unicode.org/charts
(Unicode Character Code Charts)
[ASCII]:
http://webstore.ansi.org/RecordDetail.aspx?sku=INCITS+4-1986[R2012]
(INCITS 4-1986[R2012])
[Windows code page 1252]:
https://msdn.microsoft.com/en-us/library/cc195054.aspx
(Code Page 1252 Windows Latin 1)
[ISO/IEC 8859-1]:
http://webstore.ansi.org/RecordDetail.aspx?sku=ISO%2fIEC+8859-1%3a1998
(ISO/IEC 8859-1:1998 Information technology - 8-bit single-byte coded graphic character sets - Part 1: Latin alphabet No. 1)
[ISO/IEC 14882:2011]:
http://www.iso.org/iso/home/store/catalogue_ics/catalogue_detail_ics.htm?csnumber=50372
(ISO/IEC 14882:2011 Information technology -- Programming languages -- C++)
[ISO/IEC 19217:2015]:
http://www.iso.org/iso/home/store/catalogue_tc/catalogue_detail.htm?csnumber=64031
(ISO/IEC technical specification 19217:2015, C++ Extensions for concepts)
[N2249]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2249.html
(N2249: New Character Types in C++)
[N2442]:
http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
(N2442: Raw and Unicode String Literals; Unified Proposal (Rev. 2))
[gcc]:
https://gcc.gnu.org
(GCC, the GNU Compiler Collection)
