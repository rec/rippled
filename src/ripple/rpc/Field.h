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

#ifndef RIPPLE_RPC_FIELD_H_INCLUDED
#define RIPPLE_RPC_FIELD_H_INCLUDED

#include <ripple/json/json_value.h>
#include <ripple/rpc/Status.h>
#include <ripple/app/ledger/Ledger.h>
#include <boost/optional.hpp>
#include <map>

namespace ripple {
namespace RPC {
namespace field {

using TypeName = std::string;
using Name = std::string;

/** Is a field required? */
enum class Required {no, yes};

/** Description describes one field. */
struct Description
{
    using Help = std::vector <std::string>;

    TypeName type;
    Required required;
    Help help;
};

template <class Type>
TypeName typeName();

template <class Type>
bool matchType (Json::Value const&);

template <class Type>
typename Type::Value getFrom (Json::Value const&, Context&);

/** A Dictionary is a collecton of named Descriptions.  */
using Dictionary = std::map <Name, Description>;


/** Types of Json fields in ripple.
    Fields fall in two categories:

     * scalar fields (Boolean, string, Hash, etc) and
     * compound fields (Ledger, Currency, etc).

     Each Field Type needs to overload the following functions:

*/

// Scalar field types.
struct Boolean
{
    using Value = bool;
};

struct String
{
    using Value = std::string;
};

struct Hash
{
    using Value = uint256;
};

// Numbers.

enum class Signed {no, yes};
enum class Size {small, big};
enum class Representation {integer, floating};

template <Signed, Size, Representation>
struct Number;

template <>
struct Number <Signed::yes, Size::small, Representation::integer>
{
    using Value = int32_t;
};

template <>
struct Number <Signed::no, Size::small, Representation::integer>
{
    using Value = uint32_t;
};

template <Signed s>
struct Number <s, Size::small, Representation::floating>
{
    using Value = double;
};

template <>
struct Number <Signed::no, Size::big, Representation::integer>
{
    using Value = int64_t;
};

template <>
struct Number <Signed::yes, Size::big, Representation::integer>
{
    using Value = uint64_t;
};

template <Signed s>
struct Number <s, Size::big, Representation::floating>
{
    using Value = STAmount;
};

using Integer = Number <Signed::yes, Size::small, Representation::integer>;
using Unsigned = Number <Signed::no, Size::small, Representation::integer>;
using Floating = Number <Signed::yes, Size::small, Representation::floating>;
using UnsignedFloating =
        Number <Signed::no, Size::small, Representation::floating>;

using BigInteger = Number <Signed::yes, Size::big, Representation::integer>;
using BigUnsigned = Number <Signed::no, Size::big, Representation::integer>;
using BigFloating = Number <Signed::yes, Size::big, Representation::floating>;
using BigUnsignedFloating =
        Number <Signed::no, Size::big, Representation::floating>;

// Compound field types.
struct Ledger
{
    using Value = ripple::Ledger::pointer;
};

struct Directory
{
    using Value = uint256;
};

struct Offer
{
    using Value = uint256;
};


/** A Field wraps a optional type so it can be set from a Json::Value.

    The parameter class `Type` must be one of the field types listed above:
       Boolean, Number<>, String, Hash, Ledger, Directory, Offer.
 */
template <class Type>
class Field
{
public:
    using Value = typename Type::Value;

    Field (Dictionary::value_type const& value)
            : name_ (value.first), desc_ (value.second)
    {
        assert (typeName <Type>() == desc_.type);
    }

    void setFrom (Json::Value const& json, Context& context)
    {
        value_ = boost::none;
        if (! json.isMember (name_)) {
            if (desc_.required == Required::yes)
                throw Status (rpcINVALID_PARAMS, "Missing field", name_);
            return;
        }

        auto& subjson = json[name_];
        if (! matchType <Type> (subjson))
            throw Status (rpcINVALID_PARAMS, "Wrong type", name_);

        value_ = getFrom <Type> (subjson, context);
    }

    Value& operator*() { return *value_; }
    Value* operator->() { return value_.get_ptr(); }
    operator bool() const { return bool (value_); }

private:
    static char const *name;
    Name const& name_;
    Description const& desc_;
    boost::optional <Value> value_;
};

} // field
} // RPC
} // ripple

#endif
