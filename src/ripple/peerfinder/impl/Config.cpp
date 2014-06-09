//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012, 2013 Ripple Labs Inc.

    Permission to use, copy, modify, and/or distribute this software for any
    purpose  with  or without fee is hereby granted, provided that the above
    copyright notice and this permission notice appear in all copies.

    THE  SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
    WITH  REGARD  TO  THIS  SOFTWARE  INCLUDING  ALL  IMPLIED  WARRANTIES  OF
    MERCHANTABILITY  AND  FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
    ANY  SPECIAL ,  DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
    WHATSOEVER  RESULTING  FROM  LOSS  OF USE, DATA OR PROFITS, WHETHER IN AN
    ACTION  OF  CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/
//==============================================================================

namespace ripple {
namespace PeerFinder {

double getOutPeers (ConfigProto const& config)
{
    auto outPeers = config.max_peers() * config.out_percent() * 0.01;
    return std::max (outPeers, double(config.min_out_count()));
}

uint32 getMaxPeers (ConfigProto const& config)
{
    return std::max(config.max_peers(), config.min_out_count());
}

void write (ConfigProto const& config, beast::PropertyStream::Map& map)
{
    // TODO(tom): this would be automated using proto reflection in the final
    // design.
    map ["max_peers"]       = getMaxPeers(config);
    map ["out_peers"]       = getOutPeers(config);
    map ["want_incoming"]   = config.want_incoming();
    map ["auto_connect"]    = config.auto_connect();
    map ["port"]            = config.listening_port();
    map ["features"]        = config.features();
}

}
}
