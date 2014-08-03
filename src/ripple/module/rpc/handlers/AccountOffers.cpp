//------------------------------------------------------------------------------
/*
    This file is part of rippled: https://github.com/ripple/rippled
    Copyright (c) 2012-2014 Ripple Labs Inc.

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

static void offerAdder (Json::Value& jvLines, SLE::ref offer)
{
    if (offer->getType () == ltOFFER)
    {
        Json::Value&    obj = jvLines.append (Json::objectValue);
        offer->getFieldAmount (sfTakerPays).setJson (obj[jss::taker_pays]);
        offer->getFieldAmount (sfTakerGets).setJson (obj[jss::taker_gets]);
        obj[jss::seq] = offer->getFieldU32 (sfSequence);
        obj[jss::flags] = offer->getFieldU32 (sfFlags);
    }
}

// {
//   account: <account>|<nickname>|<account_public_key>
//   account_index: <number>        // optional, defaults to 0.
//   ledger_hash : <ledger>
//   ledger_index : <ledger_index>
// }
Json::Value doAccountOffers (RPC::Context& context)
{
    Ledger::pointer ledger;
    Json::Value result
            = RPC::lookupLedger (context.params_, ledger, context.netOps_);

    if (!ledger)
        return result;

    if (!context.params_.isMember (jss::account))
        return RPC::missing_field_error ("account");

    std::string strIdent = context.params_[jss::account].asString ();
    bool bIndex = context.params_.isMember (jss::account_index);
    int iIndex = bIndex ? context.params_[jss::account_index].asUInt () : 0;

    RippleAddress   raAccount;

    result = RPC::accountFromString (
        ledger, raAccount, bIndex, strIdent, iIndex, false, context.netOps_);

    if (!result.empty ())
        return result;

    // Get info on account.

    result[jss::account] = raAccount.humanAccountID ();

    if (bIndex)
        result[jss::account_index]   = iIndex;

    if (!ledger->hasAccount (raAccount))
        return rpcError (rpcACT_NOT_FOUND);

    Json::Value& jvsOffers = (result[jss::offers] = Json::arrayValue);
    auto adder = std::bind (
        &offerAdder,
        std::ref (jvsOffers),
        std::placeholders::_1);
    ledger->visitAccountItems (raAccount.getAccountID (), adder);
    context.loadType_ = Resource::feeMediumBurdenRPC;

    return result;
}

} // ripple
