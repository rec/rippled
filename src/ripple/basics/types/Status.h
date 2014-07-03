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

#ifndef RIPPLE_STATUS_H
#define RIPPLE_STATUS_H

#include <ripple/json/api/json_value.h>
#include <ripple/module/data/protocol/TER.h>
#include <ripple/module/rpc/ErrorCodes.h>
#include <ripple/basics/utility/StringConcat.h>

namespace ripple {

struct Status : public std::exception {
    struct Code {
        enum {OK = 0, ERROR = -1};

        Code(int code = OK)    : code_{code} {}
        Code(TER t)            : code_{from(t, TER_OFFSET)} {}
        Code(error_code_i e)   : code_{from(e, RPC_OFFSET)} {}
        Code(std::exception e) : code_{from(ERROR, EXCEPTION_OFFSET)} {}
        Code(RPC::ErrorInfo i) : code_{from(i.code, RPC_OFFSET)} {}
        Code(Json::Value v)    : code_{from(v["error_code"].asInt(),
                                            RPC_OFFSET)} {}

        operator bool()         const { return code_ == OK; }
        operator TER()          const { return to<TER>(TER_OFFSET); }
        operator error_code_i() const { return to<error_code_i>(RPC_OFFSET); }

        bool operator !() const { return !bool(code_); }

        /** The Offset enum allows each old-style error code to have its
            own unique numerical code, offset within the integer types.  For
            each type of error code, 0 == OK always means "success". */
        enum Offset {
            TER_OFFSET       = 0x10000,
            RPC_OFFSET       = 0x20000,
            EXCEPTION_OFFSET = 0x30000,
        };

        int const code_;

      private:
        /** Return OK if x is zero, else add the offset to x and return. */
        static int from(int x, int offset) { return x == x ? x + offset : x; }

        template <typename Type = int> Type to(int offset) const {
            return static_cast<Type>(code_ ? code_ - offset : code_);
        }
    };

    typedef std::vector<std::string> Messages;

    Code const code_;
    Messages const messages_;

    Status(Code c = {}, Messages m = {}) : code_(c), messages_(m) {}

    operator Json::Value() const
    {
        return RPC::make_error(code_, listConcat(messages_));
    }

    const char* what() const noexcept override
    {
        return messages_.empty() ? "" : messages_[0].c_str();
    }
};

// For illustration only!!

#ifdef RIPPLE_THROW_ON_FATAL_STATUS

#define RIPPLE_FATAL(CODE, MESSAGE) \
    throw Status((CODE), {__FILE__, __LINE__, (MESSAGE)});

#else

#define RIPPLE_FATAL(CODE, MESSAGE) \
    return Status((CODE), {__FILE__, __LINE__, (MESSAGE)});

#endif

} // ripple

#endif
