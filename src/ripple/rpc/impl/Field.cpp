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

namespace ripple {
namespace RPC {
namespace field {

template <>
TypeName typeName <Boolean>()
{
    return "Boolean";
}

template <>
bool matchType <Boolean> (Json::Value const& json)
{
    return json.isBool();
}

template <>
bool getFrom <Boolean> (Json::Value const& json, Context&)
{
    return json.asBool();
}

template <>
TypeName typeName <String>()
{
    return "String";
}

template <>
bool matchType <String> (Json::Value const& json)
{
    return json.isString();
}

template <>
String::Value getFrom <String> (Json::Value const& json, Context&)
{
    return json.asString();
}

template <>
TypeName typeName <Hash>()
{
    return "Hash";
}

template <>
bool matchType <Hash> (Json::Value const& json)
{
    return json.isString();
}

template <>
Hash::Value getFrom <Hash> (Json::Value const& json, Context&)
{
    Hash::Value hash;
    if (hash.SetHex (json.asString()))
        return hash;
    throw Status (rpcINVALID_PARAMS, "hashMalformed");
}

} // field
} // RPC
} // ripple
