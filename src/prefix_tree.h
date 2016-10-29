#pragma once
#include <array>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>
#include "types.h"

namespace code_directory {

/**
 * \brief Basic Node for tree
 * This node contains 10 pointers to its children and its parent.
 *
 */
template <class Value>
class Node {
public:
    typedef Node<Value> self_t;
    typedef std::unique_ptr<self_t> self_ptr;

    Node() = delete;
    Node(const self_t &) = delete;

    Node(const code_string &code, const Value &data) :
        _children{ {nullptr} },
        _data(data),
        _code(code),
        _parent { nullptr }
    {
    }
    Node(code_string &&code, Value &&data) :
        _children{ {nullptr} },
        _data(std::move(data)),
        _code(std::move(code)),
        _parent { nullptr }
    {
    }

    // Node owns its children
    ~Node() {
        for (auto &child: _children) {
            if (child) {
                child.reset();
            }
        }
    }

    void set_child(size_t index, self_ptr &&child) {
        _children[index] = std::move(child);
        _children[index]->_parent = this;
    }


    const self_t *get_child(size_t index) const {
        return _children[index].get();
    }

    Value &data() {
        return _data;
    }
    const Value &data() const {
        return _data;
    }

    const code_string &code() const {
        return _code;
    }

    const self_t *parent() const {
        return _parent;
    }

    template<class Visitor>
    void accept(Visitor &visitor) const {
        if (!visitor.visit(*this)) {
            return;
        }
        for (auto &child: _children) {
            if (child != nullptr) {
                child->accept(visitor);
            }
        }
    }

#ifndef DEBUG
protected:
#endif
    std::array<self_ptr, 10> _children;
    Value _data;
    code_string _code;
    self_t *_parent;
};

/**
 * \brief Container that allows element searches by maximum matching prefix.
 *
 * PrefixTree stores data as a tree and provides retrieve_maximum_prefix_node method
 * which searches for node that has maximum matching prefix for provided code.
 * PrefixTree can be used for concurrent searches and single-threaded inserts.
 */
template <class Value, bool (*Empty)(const Value&) = is_empty>
class PrefixTree {
public:
    typedef PrefixTree<Value, Empty> self_t;
    typedef Node<Value> node_t;
    typedef Value data_t;
    typedef std::function<bool (const Value&)> tester_t;

    PrefixTree(const Value& empty = Value{}) :
        _root_node(get_empty<code_string>(), empty),
        _empty(empty)
    {
    }
    PrefixTree(const PrefixTree<Value>&) = delete;

    /**
     * \brief Inserts or updates data in the tree.
     * Tries to find Node for specified key in the tree and updates value in it.
     * Adds new Node(s) as necessary if exact match could not be found.
     *
     * \param code Code which data belongs to
     * \param value Data to be inserted
     */
    void put_data(const code_string &code,
                  const Value &data,
                  tester_t tester_function = [](const Value&){return true;}) {

        size_t matching_len;
        node_t *cur_node = maximum_matching_node(code, &matching_len);
        if (code.length() == matching_len) {
            // Node already exists, just update data in it if necessary
            if (tester_function(cur_node->data())) {
                cur_node->data() = data;
            }
        }
        else {
            // No such node exists; create all interposing nodes
            for (; code.length() > matching_len; ++matching_len) {
                auto new_code = code.substr(0, matching_len + 1);
                auto new_node = typename node_t::self_ptr{new node_t(new_code, _empty)};
                auto tmp = new_node.get();
                cur_node->set_child(code[matching_len], std::move(new_node));
                cur_node = tmp;
            }
            cur_node->data() = data;
        }
    }

    node_t *maximum_matching_node(const code_string &code, size_t *match) {
        /*
         * Solution from Effective C++ by Scott Meyers
         * "Avoid Duplication in const and Non-const Member Function,"
         * on p. 23, in Item 3 "Use const whenever possible," in Effective C++
         * */
        return const_cast<node_t *>(static_cast<const self_t*>(this)->maximum_matching_node(code, match));
    }


    /**
     * \brief Searches for the maximum matching prefix Node in the tree.
     *
     * \param[in] code Code to search for
     * \param[out] match Count of symbols successfully matched. 0 means no symbols matched
     * \return Non-null pointer to node that has maximum matching prefix.
     */
    const node_t *maximum_matching_node(const code_string &code, size_t *match) const {
        size_t index = 0;
        const node_t *cur = &_root_node, *next = nullptr;
        while ((code.length() > index) &&
               ((next = cur->get_child(code[index])) != nullptr) ) {
            ++index;
            cur = next;
        }
        *match = index;
        return cur;
    }

    /**
     * @brief Search for exactly matching node
     * @param code Code to search for
     * @return Pointer to found node or nullptr if search is unsuccessfull
     */
    const node_t *exactly_matching_node(const code_string &code) const {
        size_t index = 0;
        auto ret = maximum_matching_node(code, &index);
        if (code.length() == index) {
            return ret;
        } else {
            return nullptr;
        }
    }

    /**
     * \brief Searches for the data of maximum matching prefix Node in the tree.
     *
     * \param[in] code Code to search for
     * \param[out] match Optional. Count of symbols successfully matched. 0 means no symbols matched
     * \return Reference to data of node that has maximum matching prefix
     */
    const Value &data_for_max_match(const code_string &code, size_t *match) const {
        const node_t *node = maximum_matching_node(code, match);
        const node_t *parent = node->parent();
        while (Empty(node->data()) && parent != nullptr) {
            node = parent;
            parent = node->parent();
        }
        return node->data();
    }

    template<class Visitor>
    void accept(Visitor &visitor) const {
        _root_node.accept(visitor);
    }

#ifndef DEBUG
private:
#endif
    node_t _root_node;
    Value _empty;
};
}
