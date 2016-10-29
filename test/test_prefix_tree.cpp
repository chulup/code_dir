#include <cstring>
#include <limits>
#include <gtest/gtest.h>

#include "src/prefix_tree.h"
#include "src/visit_stats.h"

using namespace code_directory;

typedef Node<int> TNode;

TEST(node, constructor) {
    TNode a{"0", 0}, b{"5", 5 };

    for(size_t i = 0; i < 10; ++i) {
        EXPECT_EQ(a.get_child(i), nullptr);
        EXPECT_EQ(b.get_child(i), nullptr);
    }
    EXPECT_EQ(a.parent(), nullptr);
    EXPECT_EQ(b.parent(), nullptr);

    EXPECT_EQ(b.data(), 5);

    b.data() = 10;
    EXPECT_EQ(b.data(), 10);
}

TEST(node, children) {
    TNode::self_ptr a{new TNode("0", 0)},
                    b{new TNode("0", 0)},
                    c{new TNode("0", 0)},
                    d{new TNode("0", 0)};
    TNode *ap = a.get(), *bp = b.get(), *cp = c.get(), *dp = d.get();


    a->set_child(1, std::move(b));
    a->set_child(5, std::move(c));
    a->set_child(0, std::move(d));

    EXPECT_EQ(a->get_child(0), dp);
    EXPECT_EQ(a->get_child(1), bp);
    EXPECT_EQ(a->get_child(2), nullptr);
    EXPECT_EQ(a->get_child(3), nullptr);
    EXPECT_EQ(a->get_child(4), nullptr);
    EXPECT_EQ(a->get_child(5), cp);
    EXPECT_EQ(a->get_child(6), nullptr);
    EXPECT_EQ(a->get_child(7), nullptr);
    EXPECT_EQ(a->get_child(8), nullptr);
    EXPECT_EQ(a->get_child(9), nullptr);

    EXPECT_EQ(bp->parent(), ap);
}

namespace code_directory {
template<>
bool is_empty<int>(const int& x) {
   return x == std::numeric_limits<int>::max();
}
template<>
int get_empty<int>() {
    return std::numeric_limits<int>::max();
}
}

typedef PrefixTree<int> TTree;

TEST(prefix_tree, constructor) {
    TTree tree;

    size_t match;
#ifdef DEBUG
    EXPECT_EQ(tree.maximum_matching_node(code_string{"0123"}, &match), &(tree._root_node));
#endif
    EXPECT_EQ(tree.exactly_matching_node(code_string{"0123"}), nullptr);
}

#define PUT_DATA(x) \
    tree.put_data(code_string{#x}, x);

TEST(prefix_tree, fill) {
    TTree tree{code_directory::get_empty<int>()};
    size_t match;

    tree.put_data(code_string{"0123"}, 123);    
    auto found = tree.maximum_matching_node(code_string{"0123"}, &match);
    EXPECT_EQ(found->code(), code_string{"0123"});
    EXPECT_EQ(found, tree.exactly_matching_node(code_string{"0123"}));
    EXPECT_EQ(tree.data_for_max_match(code_string{"0123"}, &match), 123);

    EXPECT_EQ(tree.maximum_matching_node(code_string{"01234"}, &match)->code(),
              code_string{"0123"});
    EXPECT_EQ(tree.maximum_matching_node(code_string{"09"}, &match)->code(),
              code_string{"0"});

    tree.put_data(code_string{"01234"}, 1234);
    EXPECT_EQ(tree.exactly_matching_node(code_string{"01234"})->data(),
              1234);

    PUT_DATA(313);
    PUT_DATA(310);
    PUT_DATA(312);
    PUT_DATA(33333333);
    EXPECT_EQ(tree.data_for_max_match(code_string{"33333333"}, &match), 33333333);
}

TEST(prefix_tree, wrong_data) {
    TTree tree{code_directory::get_empty<int>()};

    EXPECT_THROW(tree.put_data({"12345678901234567"}, 123), std::invalid_argument);
    EXPECT_NO_THROW(tree.put_data({"1234567890123456"}, 123));

    EXPECT_THROW(tree.put_data({"-"}, 123), std::invalid_argument);
    EXPECT_THROW(tree.put_data({"a"}, 123), std::invalid_argument);
    EXPECT_THROW(tree.put_data({"$"}, 123), std::invalid_argument);
    EXPECT_THROW(tree.put_data({"-6"}, 123), std::invalid_argument);
    EXPECT_THROW(tree.put_data({"6a"}, 123), std::invalid_argument);
    EXPECT_THROW(tree.put_data({"."}, 123), std::invalid_argument);
}

class StatsInt {
public:
    StatsInt() {
        count = 0;
        with_data = 0;
        sum = 0;
    }
    bool visit(const TTree::node_t &node) {
        count++;
        if (!is_empty(node.data())) {
            with_data++;
            sum += node.data();
        }
        return true;
    }
    int count;
    int with_data;
    int sum;
};

TEST(prefix_tree, visitor) {
    TTree tree1 { code_directory::get_empty<int>() }, tree2  { code_directory::get_empty<int>() };

    {
        auto &tree = tree1;
        PUT_DATA(313);
        PUT_DATA(3);
        PUT_DATA(32);
        PUT_DATA(1);
        PUT_DATA(000);
    }
    {
        auto &tree = tree2;
        PUT_DATA(001);
        PUT_DATA(002);
        PUT_DATA(2);
        PUT_DATA(1);
    }

    StatsInt global;
    {
        StatsInt local;
        code_directory::VisitAggregator<TTree::node_t, StatsInt, StatsInt> agg(global, local);

        tree1.accept(agg);
        EXPECT_EQ(local.count, 9);
        EXPECT_EQ(local.with_data, 5);
        EXPECT_EQ(local.sum, 349);

        EXPECT_EQ(global.count, 9);
        EXPECT_EQ(global.with_data, 5);
        EXPECT_EQ(global.sum, 349);
    }
    {
        StatsInt local;
        code_directory::VisitAggregator<TTree::node_t, StatsInt, StatsInt> agg(global, local);

        tree2.accept(agg);
        EXPECT_EQ(local.count, 7);
        EXPECT_EQ(local.with_data, 4);
        EXPECT_EQ(local.sum, 6);

        EXPECT_EQ(global.count, 16);
        EXPECT_EQ(global.with_data, 9);
        EXPECT_EQ(global.sum, 355);
    }
}
