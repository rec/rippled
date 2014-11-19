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

#ifndef RIPPLED_RIPPLE_BASICS_IMMUTABLE_H
#define RIPPLED_RIPPLE_BASICS_IMMUTABLE_H

namespace ripple {

/**
   Immutable is a templated class of cached, immutable values, where common
   operations like comparisons and to_string are pre-computed for speed.

   Each Immutable has a unique "index string" - the value that it is typically
   constructed with.  It also has a "human string" - the human-readable form of
   the class - which by default is the same as the index string.
 */
template <class Type>
class Immutable
{
public:
    ~Immutable();

    bool operator== (Immutable<Type> const& other) const;
    bool operator!= (Immutable<Type> const& other) const;

    bool operator== (beast::Zero) const;
    bool operator!= (beast::Zero) const;

    std::string const& to_string () const;
    std::string const& to_human_string () const;

    static Immutable<Type> from_string (std::string const&);

    /** Make a human-readible string from a value.
        Returning the empty string means "use the index string". */
    static std::string humanString(Type const&)
    {
        return std::string();
    }

    static Type makeValue(std::string const& s)
    {
        return Type(s);
    }

private:
    Immutable();

    struct Value;
    std::shared_ptr <ImmutableValue<T>> value_;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
// Implementation follows.

template <class Type>
struct Immutable::Value
{
    Value value_;
    std::string string_;
    std::string humanString_;
    bool isZero_;
};

template <class Type>
Immutable<Type> Immutable<Type>::from_string (std::string const& s);


        : value_(makeValue (s)),
          string_(s),
          humanString_(humanString(s)),
          isZero_(value == beast::zero)
{
}

    bool isZero() const { return isZero_; }


private:
    static std::string humanString(std::string const&)
    {
        return std::string();
    }

    static Type makeValue(std::string const& s)
    {
        return Type(s);
    }


template <class T>
class Immutable
{
public:
    Immutable() = delete;

    bool operator== (Immutable<T> const& other) const
    {
        return value_.get() == other.value_.get();
    }
    bool operator!= (Immutable<T> const& other) const
    {
        return ! (*this == other);
    }

    bool operator== (beast::Zero) const { return isZero_; }
    bool operator!= (beast::Zero) const { return !isZero_; }

private:
    std::shared_ptr<T> value_;
    std::string string_;
    std::string humanString_;
    bool const isZero_;
};


} // ripple

#endif
