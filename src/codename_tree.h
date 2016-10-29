#pragma once

#include <unordered_map>
#include <vector>
#include "prefix_tree.h"
#include "types.h"

namespace code_directory {

class CodenameTree {
public:
    typedef PrefixTree<codename_t> tree_t;
    typedef std::vector<code_string> code_list_t;

    CodenameTree()
    {

    }

    void add_code(const code_string &code, const codename_t &codename) {
        _tree.put_data(code, codename);
        _codes_list[codename].push_back(code);
    }

    const code_list_t &codes_for_name(const codename_t &codename) const {
        auto found = _codes_list.find(codename);
        if (found != _codes_list.end()) {
            return found->second;
        } else {
            throw std::out_of_range(std::string("Can't find codename ") + codename);
        }
    }

    bool is_code_for_name(const code_string &code, const codename_t &codename) const {
        auto found = _codes_list.find(codename);
        if (found == _codes_list.end() || found->second.size() == 0) {
            return false;
        }
        size_t match;
        auto node = _tree.maximum_matching_node(code, &match);
        while (node != nullptr && is_empty(node->data())) {
            node = node->parent();
        }
        if (node == nullptr) {
            return false;
        }
        return node->data() == codename;
    }

    template<class Visitor>
    void accept(Visitor &visitor) {
        _tree.accept(visitor);
    }

    std::vector<std::string> list_codenames() const {
        std::vector<std::string> ret;
        ret.reserve(_codes_list.size());
        for (const auto &code: _codes_list) {
            ret.push_back(code.first);
        }
        return ret;
    }


private:
    tree_t _tree;
    std::unordered_map<codename_t, code_list_t> _codes_list;
};

}
