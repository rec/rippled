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

#ifndef RIPPLE_JSON_UNION_H_INCLUDED
#define RIPPLE_JSON_UNION_H_INCLUDED

#include <ripple/json/Object.h>

namespace Json {

class Union
{
public:
    /** Factory function for unions. */
    static Union makeUnion (Writer&);

    Union (Union&& c);
    Union& operator= (Union&& c);

    Union (Union const& c) = delete;
    Union& operator= (Union const& c) = delete;

    class Proxy;

    // operator[] throws an exception if type() is not objectValue.
    Proxy operator[] (std::string const& key);
    Proxy operator[] (Json::StaticString const& key);

    /** Append a scalar to this union. */
    template <typename Scalar>
    void append (Scalar const&);

    /** Append a Json::Value to this union.  If the value is an objectValue or
        an arrayValue, then the result will be a modifiable Union. */
    Union append (Value const&);

private:
    Union (Array&&);
    Union (Object&&);
    Union (ValueType);

    // Throws an exception if this union doesn't have the given type.
    void checkType (ValueType);

    std::unique_ptr <Array> array_;
    std::unique_ptr <Object> object_;

    ValueType type_;
};

//------------------------------------------------------------------------------
// Implementation details.

template <typename Scalar>
void Union::append (Scalar const& s)
{
    checkType (arrayValue);
    assert (array_);
    (*array_).append (s);
}

// Detail class for Object::operator[].
class Union::Proxy : Object::Proxy
{
public:
    Proxy (Object& o, StaticString s) : Object::Proxy (o, std::string (s))
    {
    }

    Proxy (Object& o, std::string const& k) : Object::Proxy (o, k)
    {
    }

    template <class T>
    void operator= (T const& t)
    {
        object_.set (key_, t);
    }

    Union operator= (Value const&);
};

} // Json

#endif
