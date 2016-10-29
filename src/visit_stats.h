#pragma once
#include <iomanip>
#include <sstream>
#include "prefix_tree.h"
#include "rate.h"

namespace code_directory {

class StatsRates {
public:
    StatsRates() {
        count = 0;
        contains_data = 0;
        child_counts.fill(0);
    }
    bool visit(const Node<Rate> &node) {
        count++;
#ifdef DEBUG
        {
            size_t node_children = 0;
            for (auto &child: node._children) {
                if (child != nullptr) {
                    node_children++;
                }
            }
            child_counts[node_children]++;
        }
#endif
        if (!is_empty(node.data())) {
            contains_data++;
        }
        return true;
    }

    std::string to_string() const {
        using namespace std;
        std::stringstream out, temp;
        out << "Node count: " <<  count << endl <<
               "Nodes with data: " << contains_data << endl;
        for (size_t childs : child_counts) {
            out << childs << ", ";
        }

        out << endl;

        return out.str();
    }

    size_t count;
    size_t contains_data;
    std::array<size_t, 10> child_counts;
};

template<class Node, class V1, class V2>
class VisitAggregator {
public:
    VisitAggregator(V1 &_v1, V2 &_v2) :
        v1(_v1),
        v2(_v2)
    {}
    bool visit(const Node &node) {
        return v1.visit(node) && v2.visit(node);
    }

    V1 &v1;
    V2 &v2;
};
}
