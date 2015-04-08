#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-DCT");
    BOOST_CHECK(GetBoolArg("-DCT"));
    BOOST_CHECK(GetBoolArg("-DCT", false));
    BOOST_CHECK(GetBoolArg("-DCT", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-DCTo"));
    BOOST_CHECK(!GetBoolArg("-DCTo", false));
    BOOST_CHECK(GetBoolArg("-DCTo", true));

    ResetArgs("-DCT=0");
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", false));
    BOOST_CHECK(!GetBoolArg("-DCT", true));

    ResetArgs("-DCT=1");
    BOOST_CHECK(GetBoolArg("-DCT"));
    BOOST_CHECK(GetBoolArg("-DCT", false));
    BOOST_CHECK(GetBoolArg("-DCT", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noDCT");
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", false));
    BOOST_CHECK(!GetBoolArg("-DCT", true));

    ResetArgs("-noDCT=1");
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", false));
    BOOST_CHECK(!GetBoolArg("-DCT", true));

    ResetArgs("-DCT -noDCT");  // -DCT should win
    BOOST_CHECK(GetBoolArg("-DCT"));
    BOOST_CHECK(GetBoolArg("-DCT", false));
    BOOST_CHECK(GetBoolArg("-DCT", true));

    ResetArgs("-DCT=1 -noDCT=1");  // -DCT should win
    BOOST_CHECK(GetBoolArg("-DCT"));
    BOOST_CHECK(GetBoolArg("-DCT", false));
    BOOST_CHECK(GetBoolArg("-DCT", true));

    ResetArgs("-DCT=0 -noDCT=0");  // -DCT should win
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", false));
    BOOST_CHECK(!GetBoolArg("-DCT", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--DCT=1");
    BOOST_CHECK(GetBoolArg("-DCT"));
    BOOST_CHECK(GetBoolArg("-DCT", false));
    BOOST_CHECK(GetBoolArg("-DCT", true));

    ResetArgs("--noDCT=1");
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", false));
    BOOST_CHECK(!GetBoolArg("-DCT", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DCT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DCT", "eleven"), "eleven");

    ResetArgs("-DCT -bar");
    BOOST_CHECK_EQUAL(GetArg("-DCT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DCT", "eleven"), "");

    ResetArgs("-DCT=");
    BOOST_CHECK_EQUAL(GetArg("-DCT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-DCT", "eleven"), "");

    ResetArgs("-DCT=11");
    BOOST_CHECK_EQUAL(GetArg("-DCT", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-DCT", "eleven"), "11");

    ResetArgs("-DCT=eleven");
    BOOST_CHECK_EQUAL(GetArg("-DCT", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-DCT", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-DCT", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-DCT", 0), 0);

    ResetArgs("-DCT -bar");
    BOOST_CHECK_EQUAL(GetArg("-DCT", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-DCT=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-DCT", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-DCT=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-DCT", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--DCT");
    BOOST_CHECK_EQUAL(GetBoolArg("-DCT"), true);

    ResetArgs("--DCT=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-DCT", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noDCT");
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", true));
    BOOST_CHECK(!GetBoolArg("-DCT", false));

    ResetArgs("-noDCT=1");
    BOOST_CHECK(!GetBoolArg("-DCT"));
    BOOST_CHECK(!GetBoolArg("-DCT", true));
    BOOST_CHECK(!GetBoolArg("-DCT", false));

    ResetArgs("-noDCT=0");
    BOOST_CHECK(GetBoolArg("-DCT"));
    BOOST_CHECK(GetBoolArg("-DCT", true));
    BOOST_CHECK(GetBoolArg("-DCT", false));

    ResetArgs("-DCT --noDCT");
    BOOST_CHECK(GetBoolArg("-DCT"));

    ResetArgs("-noDCT -DCT"); // DCT always wins:
    BOOST_CHECK(GetBoolArg("-DCT"));
}

BOOST_AUTO_TEST_SUITE_END()
