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

#include <ripple/rpc/impl/FieldReader.h>
#include <ripple/rpc/impl/AccountFromString.h>
#include <ripple/rpc/impl/LookupLedger.h>

namespace ripple {
namespace RPC {

namespace {

void expectedFieldError (FieldRequest const& req, std::string const& message)
{
    req.reader.error = expected_field_error (req.field, message);
}

} // namespace

void readImpl (bool& result, FieldRequest const& req)
{
    if (req.value.isBool())
        result = req.value.asBool();
    else
        expectedFieldError (req, "bool");
}

void readImpl (std::string& result, FieldRequest const& req)
{
    if (req.value.isString())
        result = req.value.asString();
    else
        expectedFieldError (req, "string");
}

void readImpl (std::vector <std::string>& result, FieldRequest const& req)
{
    auto& value = req.value;
    if (value.isString())
    {
        result.push_back (value.asString());
        return;
    }

    if (! (value.isArray() && value.size()))
    {
        // An empty array is an error, because if you allowed an empty array,
        // you might as well make this field optional.
        expectedFieldError (req, "list of strings");
        return;
    }

    for (auto& v: value)
    {
        if (!v.isString())
        {
            req.reader.error = expected_field_error (
                req.field, "list of strings");
            return;
        }
        result.push_back (v.asString());
    }
}

bool readLedger (
    FieldReader& reader, Ledger::pointer& result, NetworkOPs& netOps)
{
    auto error = lookupLedger (reader.params, result, netOps);
    if (result)
        return true;
    reader.error = error;
    return false;
}

void readImpl (AccountID& result, FieldRequest const& req)
{
    std::string account;
    readImpl (account, req);
    if (req.reader.error.empty())
        readAccount (req.reader.error, result, account);
}

bool readAccount (
    Json::Value& error, AccountID& result, std::string const& value)
{
    RippleAddress ra;
    if (ra.setAccountPublic (value))
    {
        result = calcAccountID (ra);
        return true;
    }

    if (auto account = parseBase58<AccountID> (value))
    {
        result = *account;
        return true;
    }

    error = make_error (rpcACT_MALFORMED);
    return false;
}

void readImpl (std::set <AccountID>& result, FieldRequest const& req)
{
    std::vector <std::string> accounts;

    readImpl (accounts, req);
    if (! req.reader.error.empty())
        return;

    for (auto a : accounts)
    {
        AccountID account;
        if (! readAccount (req.reader.error, account, a))
            return;
        result.insert (std::move (account));
    }
}

bool readAccountAddress (FieldReader& reader, AccountID& result)
{
    bool strict = false;
    std::string name;
    if (readOptional (reader, strict, jss::strict) &&
        readRequired (reader, name, jss::account))
    {
        reader.error = accountFromString (result, name, strict);
    }
    return reader.error.empty();
}

} // RPC
} // ripple
