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

#ifndef RIPPLE_RPC_NEW_MANAGER_H_INCLUDED
#define RIPPLE_RPC_NEW_MANAGER_H_INCLUDED

#include <ripple/module/rpc/HelpDetail.h>
#include <ripple/module/rpc/Request.h>

namespace ripple {
namespace RPC {

class FunctionManager
{
public:
    typedef std::function <void (Request&)> RequestHandler;
    typedef std::function <std::string (HelpDetail)> Help;
    typedef std::pair <std::string, bool> HelpResponse;

    void add (std::string const& name, RequestHandler&& handler, Help&& help)
    {
        auto result =
                map_.emplace (std::piecewise_construct,
                              std::forward_as_tuple(name),
                              std::forward_as_tuple(
                                  std::move (handler),
                                  std::move (help)));
        assert (result.second);
    }

    /** Add a handler for the specified JSON-RPC command.

        The handler needs the following generic methods,
            void handle(Request&);
            std::string help(HelpDetail);
     */
    template <class Handler>
    void add (std::string const& name)
    {
        RequestHandler handler = [](Request& req)
        {
            Handler h;
            h.handle (req);
        };
        Help help = [](HelpDetail d)
        {
            Handler h;
            return h.help (d);
        };
        add (name, std::move(handler), std::move(help));
    }

    /** Handles incoming requests, returns false if it didn't find the handler.
     */
    bool handle (std::string const& name, Request& req) const
    {
        auto i = map_.find(name);
        if (i == map_.end())
            return false;
        i->second.first(req);
        return true;
    }

    /** Requests help at a given detail level.
     * @returns a str, bool pair - if the bool is false, then the handler name
     * wasn't found. */
    HelpResponse help (std::string const& name, HelpDetail detail) const
    {
        HelpResponse response;
        auto i = map_.find(name);
        if (i == map_.end())
        {
            response.second = false;
        }
        else
        {
            response.first = i->second.second(detail);
            response.second = true;
        }
        return response;
    }

  private:
    typedef std::pair <RequestHandler, Help> MapEntry;
    typedef std::map <std::string, MapEntry> Map;

    Map map_;
};

inline std::unique_ptr <FunctionManager> makeFunctionManager()
{
    auto result = std::make_unique <FunctionManager>();
#ifdef USE_FUNCTION
    result->add<DoPrint> ();
#endif
    return std::move (result);
}

} // RPC
} // ripple

#endif
