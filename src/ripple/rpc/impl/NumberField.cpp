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

namespace {

std::string to_string (Signed s)
{
    return s == Signed::no ? "Signed" : "Unsigned";
}

std::string to_string (Size s)
{
    return s == Size::small ? "Small" : "Big";
}

std::string to_string (Representation r)
{
    return r == Representation::integer ? "Integer" : "Floating";
}

template <Signed sign, Size size, Representation rep>
TypeName getTypeName()
{
    return to_string (sign) + to_string (size) + to_string(rep);
}

template <Signed sign, Size size, Representation rep>
bool getMatchType (Json::Value const& v)
{
    if (size == Size::big)
        return v.isString();
    if (rep == Representation::floating)
        return v.isNumeric() && (sign == Signed::yes || v.asDouble() >= 0.0);
    return v.isIntegral() && (sign == Signed::yes || v.asInt() >= 0.0);
}

} // namespace

template <>
TypeName typeName <Integer>()
{
    return getTypeName <Signed::yes, Size::small, Representation::integer>();
}

template <>
bool matchType <Integer>(Json::Value const& v)
{
    return getMatchType <Signed::yes, Size::small, Representation::integer>(v);
}

template <>
Integer::Value getFrom <Integer> (Json::Value const& v, Context&)
{
    return v.asInt();
}

template <>
TypeName typeName <Unsigned>()
{
    return getTypeName <Signed::no, Size::small, Representation::integer>();
}

template <>
bool matchType <Unsigned>(Json::Value const& v)
{
    return getMatchType <Signed::no, Size::small, Representation::integer>(v);
}

template <>
Unsigned::Value getFrom <Unsigned>(Json::Value const& v, Context&)
{
    return v.asUInt();
}

template <>
TypeName typeName <Floating>()
{
    return getTypeName <Signed::yes, Size::small, Representation::floating>();
}

template <>
bool matchType <Floating>(Json::Value const& v)
{
    return getMatchType <Signed::yes, Size::small, Representation::floating>(v);
}

template <>
Floating::Value getFrom <Floating>(Json::Value const& v, Context&)
{
    return v.asDouble();;
}

template <>
TypeName typeName <UnsignedFloating>()
{
    return getTypeName <Signed::no, Size::small, Representation::floating>();
}

template <>
bool matchType <UnsignedFloating>(Json::Value const& v)
{
    return getMatchType <Signed::no, Size::small, Representation::floating>(v);
}

template <>
UnsignedFloating::Value getFrom <UnsignedFloating>(Json::Value const& v, Context&)
{
    return v.asDouble();
}

template <>
TypeName typeName <BigInteger>()
{
    return getTypeName <Signed::yes, Size::big, Representation::integer>();
}

template <>
bool matchType <BigInteger>(Json::Value const& v)
{
    return getMatchType <Signed::yes, Size::big, Representation::integer>(v);
}

template <>
BigInteger::Value getFrom <BigInteger>(Json::Value const& v, Context&)
{
    return std::stoll (v.asString());
}

template <>
TypeName typeName <BigUnsigned>()
{
    return getTypeName <Signed::no, Size::big, Representation::integer>();
}

template <>
bool matchType <BigUnsigned>(Json::Value const& v)
{
    return getMatchType <Signed::no, Size::big, Representation::integer>(v);
}

template <>
BigUnsigned::Value getFrom <BigUnsigned>(Json::Value const& v, Context&)
{
    return std::stoll (v.asString());
}

template <>
TypeName typeName <BigFloating>()
{
    return getTypeName <Signed::yes, Size::big, Representation::floating>();
}

template <>
bool matchType <BigFloating>(Json::Value const& v)
{
    return getMatchType <Signed::yes, Size::big, Representation::floating>(v);
}

template <>
BigFloating::Value getFrom <BigFloating>(Json::Value const& v, Context&)
{
    return STAmount {}; // TODO
}

template <>
TypeName typeName <BigUnsignedFloating>()
{
    return getTypeName <Signed::no, Size::big, Representation::floating>();
}

template <>
bool matchType <BigUnsignedFloating>(Json::Value const& v)
{
    return getMatchType <Signed::no, Size::big, Representation::floating>(v);
}

template <>
BigUnsignedFloating::Value getFrom <BigUnsignedFloating> (
    Json::Value const& v, Context&)
{
    return STAmount {};
}

} // field
} // RPC
} // ripple
