// This file Copyright © Mnemosyne LLC.
// It may be used under GPLv2 (SPDX: GPL-2.0-only), GPLv3 (SPDX: GPL-3.0-only),
// or any future license endorsed by Mnemosyne LLC.
// License text can be found in the licenses/ folder.

#include <exception>
#include <iostream>
#include <locale>
#include <optional>
#include <stdexcept>

#include "libtransmission/utils.h"

#if defined(_WIN32) && defined(__clang_analyzer__)
// See https://github.com/llvm/llvm-project/issues/44701
#define WORKAROUND_CLANG_TIDY_GH44701
#endif

std::optional<std::locale> tr_locale_set_global(char const* locale_name) noexcept
{
#ifndef WORKAROUND_CLANG_TIDY_GH44701
    try
#endif
    {
        return tr_locale_set_global(std::locale{ locale_name });
    }
#ifndef WORKAROUND_CLANG_TIDY_GH44701
    catch (std::runtime_error const&)
    {
        return {};
    }
#endif
}

std::optional<std::locale> tr_locale_set_global(std::locale const& locale) noexcept
{
#ifndef WORKAROUND_CLANG_TIDY_GH44701
    try
#endif
    {
        auto old_locale = std::locale::global(locale);

#ifndef TR_EMBEDDED_SIZE_OPTIMIZATIONS
        std::cout.imbue(std::locale{});
        std::cerr.imbue(std::locale{});
#endif

        return old_locale;
    }
#ifndef WORKAROUND_CLANG_TIDY_GH44701
    catch (std::exception const&)
    {
        return {};
    }
#endif
}
