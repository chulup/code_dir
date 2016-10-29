#include <gtest/gtest.h>

#include "src/vendor_tree.h"

using namespace code_directory;

class StatsVendor {
public:
    StatsVendor() {
        count = 0;
        with_data = 0;
    }
    bool visit(const VendorTree::node_t &node) {
        count++;
        if (!is_empty(node.data())) {
            with_data++;
        }
        return true;
    }
    int count;
    int with_data;
};

TEST(vendor, empty) {
    VendorTree tree;

    EXPECT_EQ(tree.max_matching_node(code_string{"0123"}),
              tree.max_matching_node(code_string{"0"}));

    EXPECT_TRUE(tree.get_maximum_prefix_rate(code_string{"34"}).is_empty());

    StatsVendor stats;
    tree.accept(stats);

    EXPECT_EQ(stats.count, 1);
    EXPECT_EQ(stats.with_data, 0);
}

TEST(vendor, with_data) {
    VendorTree tree;
    tree.add_rate({"86"}, {"0.01"}, 0, 1);
    tree.add_rate({"06755"}, {"0.012"}, 0, 1);

    EXPECT_EQ(tree.get_maximum_prefix_rate({"86"}), rate_string{"0.01"});
    EXPECT_EQ(tree.get_maximum_prefix_rate({"06755"}), rate_string{"0.012"});

    EXPECT_NE(tree.get_maximum_prefix_rate({"86"}), rate_string{"1."});
    EXPECT_NE(tree.get_maximum_prefix_rate({"06755"}), rate_string{"0.01"});

    EXPECT_EQ(tree.get_maximum_prefix_rate({"868"}), rate_string{"0.01"});
    EXPECT_EQ(tree.get_maximum_prefix_rate({"067550"}), rate_string{"0.012"});

    EXPECT_TRUE(tree.get_maximum_prefix_rate({"8"}).is_empty());
    EXPECT_TRUE(tree.get_maximum_prefix_rate({"0675"}).is_empty());

    EXPECT_EQ(tree.get_maximum_prefix_rate({"868"}),
              tree.get_maximum_prefix_rate({"869"}));
    EXPECT_EQ(tree.get_maximum_prefix_rate({"067551"}),
              tree.get_maximum_prefix_rate({"067552"}));

    EXPECT_EQ(tree.max_matching_node({"868"}),
              tree.max_matching_node({"869"}));
    EXPECT_EQ(tree.max_matching_node({"067551"}),
              tree.max_matching_node({"067552"}));

    StatsVendor stats;
    tree.accept(stats);

    EXPECT_EQ(stats.count, 8);
    EXPECT_EQ(stats.with_data, 2);
}

TEST(vendor, dates) {
    VendorTree tree;
    tree.add_rate({"86"}, {"0.01"}, 10, 20);
    EXPECT_EQ(tree.get_maximum_prefix_rate({"86"}), rate_string{"0.01"});

    tree.add_rate({"86"}, {"0.02"}, 9, 20);
    EXPECT_EQ(tree.get_maximum_prefix_rate({"86"}), rate_string{"0.01"});

    tree.add_rate({"86"}, {"0.03"}, 11, 20);
    EXPECT_EQ(tree.get_maximum_prefix_rate({"86"}), rate_string{"0.03"});
}

TEST(vendor, wrong_data) {
    VendorTree tree;

    EXPECT_THROW(tree.add_rate({"-86"}, {"0.12"}, 0, 1), std::invalid_argument);
}
