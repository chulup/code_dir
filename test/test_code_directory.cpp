#include <gtest/gtest.h>

#include "src/code_directory.h"

using namespace code_directory;

const VendorId idA = {1};
const VendorId idB = {2};
const VendorId idC = {3};

void fill_directory(CodeDirectory &directory) {
    {
        auto codename = boost::make_shared<CodenameTree>();
        codename->add_code({"86"},  {"China Proper"});
        codename->add_code({"8613"}, {"China Mobile"});
        codename->add_code({"8620"}, {"China Proper"});
        codename->add_code({"8653"}, {"China CNC"});
        codename->add_code({"867"}, {"Example"});
        directory.set_codename_tree(codename);
    }
    {
        auto vendorA = boost::make_shared<VendorTree>();
        vendorA->add_rate({"86"},       {"0.005"}, 0, 1);
        vendorA->add_rate({"86755"},    {"0.004"}, 0, 1);
        vendorA->add_rate({"8621"},     {"0.003"}, 0, 1);
        vendorA->add_rate({"8620"},     {"0.002"}, 0, 1);
        vendorA->add_rate({"862010"},   {"0.001"}, 0, 1);
        vendorA->add_rate({"8610"},     {"0.006"}, 0, 1);
        directory.set_vendor_tree(idA, vendorA);
    }
    {
        auto vendorB = boost::make_shared<VendorTree>();
        vendorB->add_rate({"86"},       {"0.002"}, 0, 1);
        directory.set_vendor_tree(idB, vendorB);
    }
    {
        auto vendorC = boost::make_shared<VendorTree>();
        vendorC->add_rate({"86"},       {"0.006"}, 0, 1);
        vendorC->add_rate({"862"},      {"0.003"}, 0, 1);
        vendorC->add_rate({"86102"},    {"0.002"}, 0, 1);
        vendorC->add_rate({"8610"},     {"0.004"}, 0, 1);
        vendorC->add_rate({"86715"},    {"0.01"}, 0, 1);
        directory.set_vendor_tree(idC, vendorC);
    }
}

TEST(directory, getRates) {
    CodeDirectory directory;
    fill_directory(directory);

    using rates_t = CodeDirectory::rates_result_t;
    typedef std::set<rates_t::value_type> set_t;
    {
        set_t a_cp;
        a_cp.emplace(set_t::key_type({"86"}, {"0.005"}));
        a_cp.emplace(set_t::key_type({"8621"}, {"0.003"}));
        a_cp.emplace(set_t::key_type({"8620"}, {"0.002"}));
        a_cp.emplace(set_t::key_type({"862010"}, {"0.001"}));
        a_cp.emplace(set_t::key_type({"8610"}, {"0.006"}));

        rate_string min, max;
        auto CP_A = directory.get_rates(idA, {"China Proper"}, &min, &max);
        auto set = decltype(a_cp)(CP_A.begin(), CP_A.end());
        EXPECT_TRUE(a_cp == set);
        EXPECT_EQ(min, rate_string{"0.001"});
        EXPECT_EQ(max, rate_string{"0.006"});
    }
    {
        set_t b_cp;
        b_cp.emplace(set_t::key_type({"86"}, {"0.002"}));

        rate_string min, max;
        auto CP_B = directory.get_rates(idB, {"China Proper"}, &min, &max);
        auto set = set_t(CP_B.begin(), CP_B.end());
        EXPECT_TRUE(b_cp == set);
        EXPECT_EQ(min, rate_string{"0.002"});
        EXPECT_EQ(max, rate_string{"0.002"});
    }
    {
        set_t c_cp;
        c_cp.emplace(set_t::key_type({"86"},      {"0.006"}));
        c_cp.emplace(set_t::key_type({"862"},     {"0.003"}));
        c_cp.emplace(set_t::key_type({"86102"},   {"0.002"}));
        c_cp.emplace(set_t::key_type({"8610"},    {"0.004"}));

        rate_string min, max;
        auto CP_C = directory.get_rates(idC, {"China Proper"}, &min, &max);
        auto set = set_t(CP_C.begin(), CP_C.end());
        EXPECT_TRUE(c_cp == set);
        EXPECT_EQ(min, rate_string{"0.002"});
        EXPECT_EQ(max, rate_string{"0.006"});
    }
}

TEST(CodeDirectory, getVendors) {
    CodeDirectory directory;
    fill_directory(directory);

    auto get_vendors = directory.get_vendors({"China Proper"});
    typedef std::set<decltype(get_vendors)::value_type> set_t;

    auto result = set_t(get_vendors.begin(), get_vendors.end());

    decltype(result) expected;
    expected.emplace(set_t::key_type(idA, {"0.001"}, {"0.006"}));
    expected.emplace(set_t::key_type(idB, {"0.002"}, {"0.002"}));
    expected.emplace(set_t::key_type(idC, {"0.002"}, {"0.006"}));

    EXPECT_EQ(result, expected);
}

TEST(CodeDirectory, remove_vendor) {
    CodeDirectory directory;
    fill_directory(directory);
    
    directory.remove_vendor(idB);
    {
        auto get_vendors = directory.get_vendors({"China Proper"});
        typedef std::set<decltype(get_vendors)::value_type> set_t;

        auto result = set_t(get_vendors.begin(), get_vendors.end());

        decltype(result) expected;
        expected.emplace(set_t::key_type(idA, {"0.001"}, {"0.006"}));
        expected.emplace(set_t::key_type(idC, {"0.002"}, {"0.006"}));

        EXPECT_EQ(result, expected);
    }
    // Try to remove it again
    directory.remove_vendor(idB);
    {
        auto get_vendors = directory.get_vendors({"China Proper"});
        typedef std::set<decltype(get_vendors)::value_type> set_t;

        auto result = set_t(get_vendors.begin(), get_vendors.end());

        decltype(result) expected;
        expected.emplace(set_t::key_type(idA, {"0.001"}, {"0.006"}));
        expected.emplace(set_t::key_type(idC, {"0.002"}, {"0.006"}));

        EXPECT_EQ(result, expected);
    }
    directory.remove_vendor(idA);
    {
        auto get_vendors = directory.get_vendors({"China Proper"});
        typedef std::set<decltype(get_vendors)::value_type> set_t;

        auto result = set_t(get_vendors.begin(), get_vendors.end());

        decltype(result) expected;
        expected.emplace(set_t::key_type(idC, {"0.002"}, {"0.006"}));

        EXPECT_EQ(result, expected);
    }
}
