// Copyright (c) 2015, Tom Honermann
//
// This file is distributed under the MIT License. See the accompanying file
// LICENSE.txt or http://www.opensource.org/licenses/mit-license.php for terms
// and conditions.

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <text_view>
#include <text_view_detail/range_based_for.hpp>

using namespace std;
using namespace std::experimental;

static const int exit_success = 0;
static const int exit_failure = 1;
static const int exit_user_error = 2;

void
usage(
    ostream &os,
    const char *progpath)
{
    const char *progname = strrchr(progpath, '/');
    progname = (progname ? progname+1 : progpath);
    os << "usage: " << progpath << endl;
    os << progname << " --encoding <encoding> <file>" << endl;
    os << "    -h, --help:" << endl;
    os << "        Displays program help." << endl;
    os << "    <file>:" << endl;
    os << "        Specifies the file to decode." << endl;
    os << "    -e, --encoding <encoding>:" << endl;
    os << "        Specifies the character encoding of <file>." << endl;
    os << "        Valid encodings are:" << endl;
    os << "            utf-8" << endl;
    os << "            utf-8-bom   (BOM ignored if present)" << endl;
    os << "            utf-16      (native endian)" << endl;
    os << "            utf-16-bom  (big endian if no BOM is present)" << endl;
    os << "            utf-16-be" << endl;
    os << "            utf-16-le" << endl;
    os << "            utf-32      (native endian)" << endl;
    os << "            utf-32-bom  (big endian if no BOM is present)" << endl;
    os << "            utf-32-be" << endl;
    os << "            utf-32-le" << endl;
}

template<Encoding E>
void
dump_code_points(
    ifstream &ifs)
{
    using CUT = typename E::codec_type::code_unit_type;
    istream_iterator<CUT> ifs_in(ifs), ifs_end;

    // FIXME: The C++11 range-based-for requires that the begin and end types
    // FIXME: be identical.  The RANGE_BASED_FOR macro is used to work around
    // FIXME: this limitation.
    auto tv = make_text_view<E>(ifs_in, ifs_end);
    RANGE_BASED_FOR (const auto &ch, tv) {
        auto csid = ch.get_character_set_id();
        cout << "0x" << hex << setw(8) << setfill('0')
             << (uint_least32_t)ch.get_code_point()
             << " (" << get_character_set_info(csid).get_name() << ")"
             << endl;
    }
}

int
main(
    int argc,
    char *argv[])
{
    const char *file_name = nullptr;
    const char *encoding = nullptr;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-h") == 0 ||
            strcmp(argv[i], "--help") == 0)
        {
            usage(cout, argv[0]);
            return exit_success;
        }
        else if (strcmp(argv[i], "-e") == 0 ||
                 strcmp(argv[i], "--encoding") == 0)
        {
            if (++i >= argc) {
                cerr << "error: --encoding operand missing." << endl;
                usage(cerr, argv[0]);
                return exit_user_error;
            }
            if (encoding) {
                cerr << "error: --encoding already specified." << endl;
                usage(cerr, argv[0]);
                return exit_user_error;
            }
            encoding = argv[i];
        }
        else {
            if (file_name) {
                cerr << "error: <file> already specified." << endl;
                usage(cerr, argv[0]);
                return exit_user_error;
            }
            file_name = argv[i];
        }
    }

    if (! file_name) {
        cerr << "error: <file> not specified." << endl;
        usage(cerr, argv[0]);
        return exit_user_error;
    }

    if (! encoding) {
        cerr << "error: --encoding not specified." << endl;
        usage(cerr, argv[0]);
        return exit_user_error;
    }

    ifstream ifs(file_name, ios_base::binary);
    if (! ifs) {
        cerr << "error: failed to open file " << file_name << "." << endl;
        return exit_failure;
    }
    ifs >> noskipws;

    try {
        if (strcmp(encoding, "utf-8") == 0) {
            dump_code_points<utf8_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-8-bom") == 0) {
            dump_code_points<utf8bom_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-16") == 0) {
            // This endianness detection requires sizeof(char16_t) == 2.
            static_assert(sizeof(char16_t) == 2, "");
            if (*((unsigned char*)u"\ufeff") == 0xFF) {
                dump_code_points<utf16le_encoding>(ifs);
            } else {
                dump_code_points<utf16be_encoding>(ifs);
            }
        }
        else if (strcmp(encoding, "utf-16-bom") == 0) {
            dump_code_points<utf16bom_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-16-be") == 0) {
            dump_code_points<utf16be_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-16-le") == 0) {
            dump_code_points<utf16le_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-32") == 0) {
            // This endianness detection requires sizeof(char32_t) == 4.
            static_assert(sizeof(char32_t) == 4, "");
            if (*((unsigned char*)U"\U0000feff") == 0xFF) {
                dump_code_points<utf32le_encoding>(ifs);
            } else {
                dump_code_points<utf32be_encoding>(ifs);
            }
        }
        else if (strcmp(encoding, "utf-32-bom") == 0) {
            dump_code_points<utf32bom_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-32-be") == 0) {
            dump_code_points<utf32be_encoding>(ifs);
        }
        else if (strcmp(encoding, "utf-32-le") == 0) {
            dump_code_points<utf32le_encoding>(ifs);
        }
        else {
            cerr << "error: unrecognized encoding: '" << encoding << "'." << endl;
            usage(cerr, argv[0]);
            return exit_user_error;
        }
    } catch (const text_runtime_error &tre) {
        cerr << "error: " << tre.what() << endl;
        return exit_failure;
    }

    return exit_success;
}
