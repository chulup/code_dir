#pragma once
#include <iostream>
#include <unordered_map>

#include "prefix_tree.h"
#include "rate.h"
#include "types.h"

namespace code_directory {
class VendorTree {
public:
    typedef PrefixTree<Rate> tree_t;
    typedef tree_t::node_t node_t;

    VendorTree()
    {

    }

    const node_t *max_matching_node(const code_string &code) const {
        size_t match;
        auto ret = tree.maximum_matching_node(code, &match);
        return match > 0 ? ret : nullptr;
    }

    const rate_string &get_maximum_prefix_rate(const code_string &code) const {
        size_t match;
        return tree.data_for_max_match(code, &match).rate;
    }

    /**
        Adds \p rate for \p code in the tree of \p vendor.

        \param vendor Target vendor
        \param code Code to modify
        \param rate Rate that is to be added to code
        */
    void add_rate(const code_string &code, const rate_string &rate, time_t effective_date, time_t end_date) {
        Rate new_rate;
        if (rate.is_empty()) {
            new_rate.set_empty();
        } else {
            new_rate.set(rate, effective_date, end_date);
        }
        tree.put_data(code,
                      new_rate,
                      [effective_date](const Rate& old) {
                            return old.is_empty() || (old.effective_date < effective_date);
                        }
        );
    }

    template<class Visitor>
    void accept(Visitor &visitor) const {
        tree.accept(visitor);
    }

private:
    tree_t tree;
};
}
