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

#include <ripple/json/Union.h>

namespace Json {

Union::Union (Union&& u) {
    *this = std::move (u);
}

Union& Union::operator= (Union&& u)
{
    array_.swap (u.array_);
    object_.swap (u.object_);
    type_ = u.type_;

    return *this;
}

Union::Union (Array&& array)
        : array_ (std::make_unique <Array> (std::move (array))),
          type_ (arrayValue)
{
}

Union::Union (Object&& object)
        : object_ (std::make_unique <Object> (std::move (object))),
          type_ (objectValue)
{
}

Union::Union (ValueType type) : type_ (type)
{
}

// operator[] throws an exception if type() is not objectValue.
Union::Proxy Union::operator[] (std::string const& key)
{
    checkType (objectValue);
    assert (object_);
    return Union::Proxy (*object_, key);
}

Union::Proxy Union::operator[] (Json::StaticString const& key)
{
    checkType (objectValue);
    assert (object_);
    return {*object_, key};
}

Union Union::append (Value const& value)
{
    checkType (arrayValue);
    auto type = value.type();
    if (type == arrayValue)
    {
        auto array = array_->appendArray();
        for (auto& i: value)
            array.append (i);
        return Union (std::move(array));
    }

    if (type == objectValue)
    {
        auto object = array_->appendObject();
        copyFrom (object, value);
        return Union (std::move (object));
    }

    array_->append (value);
    return Union (type);
}

void Union::checkType (ValueType type) {
    if (type != type_)
    {
        throw JsonException (
            "Expected type " + std::to_string (type) +
            " but got " + std::to_string (type_));
    }
}

// static
Union Union::makeUnion (Writer& writer)
{
    return Union (Object::Root (writer));
}

Union Union::Proxy::operator= (Value const& value)
{
    auto type = value.type();
    if (type == arrayValue)
    {
        auto array = object_.setArray (key_);
        for (auto& i: value)
            array.append (i);
        return Union (std::move (array));
    }

    if (type == objectValue)
    {
        auto object = object_.setObject (key_);
        copyFrom (object, value);
        return Union (std::move (object));
    }

    object_.set (key_, value);
    return Union (type);
}

} // Json
