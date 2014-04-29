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

#include "../api/VerifyJson.h"
#include "../../beast/beast/unit_test/suite.h"

namespace ripple {
namespace RPC {

namespace {

typedef std::unordered_map<std::string, Json::ValueType> ValueTypeNames;

ValueTypeNames makeTypeNames() {
    ValueTypeNames typeNames;

    typeNames["null"] = Json::nullValue;
    typeNames["int"] = Json::intValue;
    typeNames["uint"] = Json::uintValue;
    typeNames["real"] = Json::realValue;
    typeNames["string"] = Json::stringValue;
    typeNames["boolean"] = Json::booleanValue;
    typeNames["array"] = Json::arrayValue;
    typeNames["object"] = Json::objectValue;

    return typeNames;
}

auto const VALUE_TYPE_NAMES = makeTypeNames();

inline std::string chopTrailingColon(std::string s)
{
    auto loc = s.find(':');
    return (loc == std::string::npos) ? s : s.substr(0, loc);
}

/** Verify a single field from a JSON value. */
inline std::string verifyOne(
    Json::Value const& value, std::string name, std::string prefix)
{
    auto loc = name.find('.');
    if (!loc || loc == name.size() - 1)  // . is the first or last character.
        return "Invalid . in verifier " + prefix + name;

    if (loc != std::string::npos)
    {
    // We have an object reference.
        auto object = name.substr(0, loc);
        if (!value.isMember(object))
            return missing_field_message(prefix + chopTrailingColon(name));

        auto const& member = value[object];
        if (!member.isObject())
            return object_field_message(prefix + object);

        auto newPrefix = prefix + name.substr(0, loc + 1);

        // Recursively verify the remaining parts.
        return verifyOne(member, name.substr(loc + 1), newPrefix);
    }

    loc = name.find(':');
    if (!loc || loc == name.size() - 1)  // : is the first or last character.
        return "Invalid : in verifier " + prefix + name;

    auto const hasType = loc != std::string::npos;
    std::string namePart = hasType ? name.substr(0, loc) : name;
    if (!value.isMember(namePart))
        return missing_field_message(prefix + chopTrailingColon(name));

    if (hasType)
    {
        auto const& typeName = name.substr(loc + 1);
        auto const& i = VALUE_TYPE_NAMES.find(typeName);
        if (i == VALUE_TYPE_NAMES.end())
        {
            return "Invalid verifier type " + typeName + " in "
                    + chopTrailingColon(prefix) + name;
        }
        if (!value[namePart].isConvertibleTo(i->second))
        {
            return "Can't convert property " + prefix + chopTrailingColon(name)
                    + " to " + typeName + ".";
        }
    }

    return "";
}

}  // namespace

Json::Value verifyJson(Json::Value const& value, std::vector<std::string> names)
{
    for (auto const& name: names)
    {
        auto message = verifyOne(value, name, "");
        if (!message.empty())
            return make_param_error(message);
    }

    return {};
}

struct JsonVerify_test : beast::unit_test::suite
{
    static std::string getErrorMessage(Json::Value const& error)
    {
        if (error.isMember("error_message"))
            return error["error_message"].asString();
        return "";
    }

    void testNoError (Json::Value const& value, std::vector<std::string> names)
    {
        auto error = verifyJson(value, names);
        expect(error.isNull(), getErrorMessage(error));
    }

    void testError (Json::Value const& value, std::string errorString,
                    std::vector<std::string> names)
    {
        auto error = verifyJson(value, names);
        expect(!error.isNull(), "No error when expected");
        auto actualError = getErrorMessage(error);
        expect(actualError == errorString, "'" + actualError + "' != '" + errorString + "'");
    }

    void testTrivial ()
    {
        testNoError(Json::Value(), {});
    }

    void testSimple ()
    {
        Json::Value value;
        value["foo"] = "bar";
        testNoError(value, {"foo"});
        testNoError(value, {"foo:string"});
        testError(value, "Missing field 'bar'.", {"bar"});
        testError(value, "Missing field 'bar'.", {"bar:string"});
        testError(value, "Can't convert property foo to int.", {"foo:int"});
    }

    void testCompound ()
    {
        Json::Value value;
        value["foo"]["bar"] = "baz";
        testNoError(value, {"foo.bar"});
        testNoError(value, {"foo.bar:string"});
        testError(value, "Missing field 'bar'.", {"bar"});
        testError(value, "Missing field 'bar'.", {"bar:string"});
    }

    void testMultiple ()
    {
        Json::Value value;
        value["foo"]["bar"] = "baz";
        value["foo"]["bing"] = false;
        value["baz"] = 2;
        testNoError(value, {"foo.bar:string", "foo.bing:boolean", "baz:int"});
        testError(value, "Missing field 'bar'.", {"bar"});
        testError(value, "Missing field 'bar'.", {"bar:string"});
    }

    void run ()
    {
        testTrivial();
        testSimple();
        testCompound();
    }
};

BEAST_DEFINE_TESTSUITE(JsonVerify, ripple_rpc, ripple);

} // RPC
} // ripple
