#include <gtest/gtest.h>

#include "src/codename_tree.h"

using namespace code_directory;

TEST(codename, empty) {
    CodenameTree tree;

    EXPECT_THROW(tree.codes_for_name({"China Proper"}), std::out_of_range);

    EXPECT_FALSE(tree.is_code_for_name({"86"}, {"China Proper"}));
    EXPECT_FALSE(tree.is_code_for_name({"86"}, {"China CNC"}));
}

TEST(codename, with_data) {
    CodenameTree tree;

    tree.add_code({"86"}, {"China Proper"});
    tree.add_code({"8613"}, {"China Mobile"});
    tree.add_code({"8620"}, {"China Proper"});
    tree.add_code({"8653"}, {"China CNC"});
    tree.add_code({"867"}, {"Example"});

    EXPECT_TRUE(tree.is_code_for_name({"86"}, {"China Proper"}));
    EXPECT_TRUE(tree.is_code_for_name({"8613"}, {"China Mobile"}));
    EXPECT_TRUE(tree.is_code_for_name({"8620"}, {"China Proper"}));
    EXPECT_TRUE(tree.is_code_for_name({"8653"}, {"China CNC"}));
    EXPECT_TRUE(tree.is_code_for_name({"867"}, {"Example"}));

    EXPECT_TRUE(tree.is_code_for_name({"861"}, {"China Proper"}));
    EXPECT_TRUE(tree.is_code_for_name({"869"}, {"China Proper"}));
    EXPECT_TRUE(tree.is_code_for_name({"861300000"}, {"China Mobile"}));

    EXPECT_FALSE(tree.is_code_for_name({"861"}, {"China CNC"}));
    EXPECT_FALSE(tree.is_code_for_name({"869"}, {"China Mobile"}));
    EXPECT_FALSE(tree.is_code_for_name({"00"}, {"Example"}));

    std::set<code_string> control;
    control.insert({"86"});
    control.insert({"8620"});

    auto codes = tree.codes_for_name({"China Proper"});
    std::set<code_string> test{codes.begin(),codes.end()};
    EXPECT_EQ(control, test);
}
