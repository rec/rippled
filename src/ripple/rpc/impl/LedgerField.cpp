//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012-2015 Ripple Labs Inc.

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

#include <ripple/rpc/Field.h>
#include <ripple/app/main/Application.h>
#include <ripple/app/misc/NetworkOPs.h>
#include <ripple/app/ledger/LedgerMaster.h>

namespace ripple {
namespace RPC {
namespace field {

namespace {

template <class Map, class Key>
auto find (Map& map, Key const& key) -> decltype (*map.find(key))
{
    auto value = map.find (key);
    if (value != map.end())
        return *value;
    throw Status (rpcINTERNAL, "Missing key in dictionary", key);
}

class LedgerImpl
{
public:
    LedgerImpl()
            : hash (find (dict, "hash")),
              index (find (dict, "index")),
              name (find (dict, "name"))
    {}

    Ledger::Value getFrom (Json::Value const&, Context&);

private:
    Field <Hash> hash;
    Field <Integer> index;
    Field <String> name;

    static Dictionary const dict;
};

Dictionary const LedgerImpl::dict
{
    {"hash", {"Hash", Required::no, {}}},
    {"index", {"Integer", Required::no, {}}},
    {"name", {"String", Required::no, {}}}
};

Ledger::Value LedgerImpl::getFrom (Json::Value const& json, Context& context)
{
    hash.setFrom (json, context);
    index.setFrom (json, context);
    name.setFrom (json, context);

    auto areSet = bool (hash) + bool (index) + bool (name);
    if (areSet > 1)
        throw Status (rpcINVALID_PARAMS,
                      "More than one of hash, index or name are set.");

    auto& netOps = context.netOps;
    if (hash)
        return netOps.getLedgerByHash (*hash);

    if (index)
        return netOps.getLedgerBySeq (*index);

    if (!name || *name == "current")
        return netOps.getCurrentLedger ();

    if (*name == "closed")
        return getApp().getLedgerMaster ().getClosedLedger ();

    if (*name == "validated")
        return netOps.getValidatedLedger ();

    throw Status (rpcINVALID_PARAMS, "ledgerIndexMalformed");
}

} // namespace

template <>
TypeName typeName <Ledger>()
{
    return "Ledger";
}

template <>
TypeName typeName <Currency>()
{
    return "Currency";
}

template <>
bool matchType <Ledger> (Json::Value const& json)
{
    return json.isObject();
}

template <>
Ledger::Value getFrom <Ledger> (Json::Value const& json, Context& context)
{
    return LedgerImpl().getFrom (json, context);
}

} // field
} // RPC
} // ripple
