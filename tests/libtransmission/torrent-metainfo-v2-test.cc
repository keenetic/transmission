// This file Copyright (C) 2026 Keenetic Limited.
// It may be used under GPLv2 (SPDX: GPL-2.0-only), GPLv3 (SPDX: GPL-3.0-only),
// or any future license endorsed by Mnemosyne LLC.
// License text can be found in the licenses/ folder.

#include <cerrno>
#include <string>
#include <string_view>

#include <libtransmission/error.h>
#include <libtransmission/torrent-metainfo.h>

#include "gtest/gtest.h"
#include "test-fixtures.h"

using namespace std::literals;

namespace libtransmission::test
{

using TorrentMetainfoV2Test = SessionTest;

namespace
{
std::string makeHybridMetainfo()
{
    auto benc = std::string{
        "d4:infod9:file treed5:a.txtd0:d6:lengthi4e11:pieces root32:"
    };

    for (int i = 0; i < 32; ++i)
    {
        benc.push_back(static_cast<char>(i));
    }

    benc +=
        "eee5:filesld6:lengthi4e4:pathl5:a.txteee12:meta versioni2e4:name11:hybrid-test"
        "12:piece lengthi32768e6:pieces20:";

    for (int i = 0; i < 20; ++i)
    {
        benc.push_back(static_cast<char>(i));
    }

    benc += "e12:piece layersdee";
    return benc;
}

std::string makeV2OnlyMetainfo()
{
    auto benc = std::string{
        "d4:infod9:file treed5:a.txtd0:d6:lengthi4e11:pieces root32:"
    };

    for (int i = 0; i < 32; ++i)
    {
        benc.push_back(static_cast<char>(i));
    }

    benc +=
        "eee12:meta versioni2e4:name7:v2-test12:piece lengthi32768ee12:piece layersdee";
    return benc;
}

std::string makeV1MetainfoWithPieceLayers()
{
    auto benc = std::string{
        "d4:infod5:filesld6:lengthi4e4:pathl5:a.txteee4:name7:v1-test12:piece lengthi32768e6:pieces20:"
    };

    for (int i = 0; i < 20; ++i)
    {
        benc.push_back(static_cast<char>(i));
    }

    benc += "e12:piece layersdee";
    return benc;
}
} // namespace

TEST_F(TorrentMetainfoV2Test, v2OnlyMetainfoIsRejected)
{
    auto error = tr_error{};
    auto metainfo = tr_torrent_metainfo{};
    EXPECT_FALSE(metainfo.parse_benc(makeV2OnlyMetainfo(), &error));
    EXPECT_EQ(EINVAL, error.code());
    EXPECT_EQ("v2-only torrent is not supported; a hybrid torrent must contain v1 pieces"sv, error.message());
}

TEST_F(TorrentMetainfoV2Test, hybridMetainfoRequiresVersionTwo)
{
    auto benc = makeHybridMetainfo();
    auto const pos = benc.find("12:meta versioni2e");
    ASSERT_NE(std::string::npos, pos);
    benc.replace(pos, std::string_view{ "12:meta versioni2e" }.size(), "12:meta versioni1e");

    auto error = tr_error{};
    auto metainfo = tr_torrent_metainfo{};
    EXPECT_FALSE(metainfo.parse_benc(benc, &error));
    EXPECT_EQ(EINVAL, error.code());
    EXPECT_EQ("invalid 'meta version'"sv, error.message());
}

TEST_F(TorrentMetainfoV2Test, v2MetainfoRequiresFileTree)
{
    auto benc = std::string{
        "d4:infod5:filesld6:lengthi4e4:pathl5:a.txteee12:meta versioni2e4:name11:hybrid-test"
        "12:piece lengthi32768e6:pieces20:"
    };

    for (int i = 0; i < 20; ++i)
    {
        benc.push_back(static_cast<char>(i));
    }

    benc += "e12:piece layersdee";

    auto error = tr_error{};
    auto metainfo = tr_torrent_metainfo{};
    EXPECT_FALSE(metainfo.parse_benc(benc, &error));
    EXPECT_EQ(EINVAL, error.code());
    EXPECT_EQ("missing 'file tree'"sv, error.message());
}

TEST_F(TorrentMetainfoV2Test, pieceLayersRequireVersionTwo)
{
    auto error = tr_error{};
    auto metainfo = tr_torrent_metainfo{};
    EXPECT_FALSE(metainfo.parse_benc(makeV1MetainfoWithPieceLayers(), &error));
    EXPECT_EQ(EINVAL, error.code());
    EXPECT_EQ("invalid 'meta version'"sv, error.message());
}

} // namespace libtransmission::test
