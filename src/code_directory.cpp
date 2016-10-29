#include "code_directory.h"

#include <atomic>
#include <set>
#include "visit_stats.h"

namespace code_directory {

CodeDirectory::CodeDirectory()
{

}

void CodeDirectory::remove_vendor(VendorId vendor) {
    auto v_row = _vendors.find(vendor);
    if (v_row == _vendors.end()) {
        return;
    }
    decltype(_vendors)::mapped_type empty_value;
    boost::atomic_store(&v_row->second, empty_value);
}


void CodeDirectory::set_vendor_tree(VendorId vendor, CodeDirectory::tree_pointer_t tree) {
    boost::atomic_store(&_vendors[vendor], tree);
}
void CodeDirectory::set_codename_tree(codename_pointer_t tree) {
    boost::atomic_store(&_codenames, tree);
}


CodeDirectory::rates_result_t CodeDirectory::get_rates(const std::string &vendor,
                                                       const std::string &code_name,
                                                       rate_string *min_rate,
                                                       rate_string *max_rate) const {
    VendorId vId = str_to_vendor(vendor);
    return get_rates(vId, code_name, min_rate, max_rate);
}

std::vector<VendorId> CodeDirectory::list_vendors() const {
    std::vector<VendorId> ret;
    ret.reserve(_vendors.size());
    for (const auto &vendor: _vendors) {
        ret.push_back(vendor.first);
    }
    return ret;
}
std::vector<std::string> CodeDirectory::list_codenames() const {
    return _codenames->list_codenames();
}

CodeDirectory::rates_result_t CodeDirectory::get_rates(VendorId vendor,
                                                       const codename_t &codename,
                                                       rate_string *min_rate,
                                                       rate_string *max_rate) const {
    typedef std::set<const VendorTree::node_t*> node_set;

    node_set roots;
    auto v_row = _vendors.find(vendor);
    if (v_row == _vendors.end()) {
        throw std::out_of_range("Vendor not found");
    }
    auto v_tree = boost::atomic_load(&(v_row->second));
    if (!v_tree) {
        min_rate->set_empty();
        max_rate->set_empty();
        return {};
    }

    // 1. for every code of China Proper, search for maximum prefix rate in vendorA
    auto codes = _codenames->codes_for_name(codename);
    for (const auto &code: codes) {
        auto node = v_tree->max_matching_node(code);
        if (node != nullptr) {
            roots.insert(node);
        }
    }

    // 2. for every code in roots, find every rate starting with that code
    node_set all_rates;

    class RatesSearch {
    public:
        RatesSearch(node_set &_nodes) :
            nodes(_nodes)
        {

        }
        bool visit(const VendorTree::node_t &node) {
            if (!is_empty(node.data())) {
                // Insert node in set if it has data
                auto inserted = nodes.insert(&node);
                // Stop traversing children nodes if node was already in a tree
                return inserted.second;
            }
            return true;
        }
        node_set &nodes;
    } rates_search { all_rates };

    for (auto &node: roots) {
        if (all_rates.count(node) == 0) {
            // add all non-empty children of node to the all_rates
            node->accept(rates_search);
        }
    }

    // 3. For every code in all_rates, search for codename with maximum
    //    prefix and take only those belonging to China Proper
   rates_result_t result;
   rate_string min;
   rate_string max;
   for (auto &node: all_rates) {
       if (_codenames->is_code_for_name(node->code(), codename)) {
           auto rate = node->data().rate;
           result.emplace_back(node->code(), rate);
           if (min.is_empty() || rate < min) {
               min = rate;
           }
           if (max.is_empty() || max < rate) {
               max = rate;
           }
       }
   }
   if (min_rate != nullptr) {
       *min_rate = min;
   }
   if (max_rate != nullptr) {
       *max_rate = max;
   }
   return result;
}

CodeDirectory::vendors_result_t CodeDirectory::get_vendors(const codename_t &code_name) const
{
    CodeDirectory::vendors_result_t result;
    for (const auto &vendor: _vendors) {
        rate_string min, max;
        get_rates(vendor.first, code_name, &min, &max);
        if (!min.is_empty() && !max.is_empty()) {
            result.emplace_back(vendor.first, min, max);
        }
    }
    return result;
}

void CodeDirectory::print_stats(bool print_all) const
{
    using namespace std;
    StatsRates global;

    for (auto &vendor: _vendors) {
        if (print_all) {
            StatsRates local;
            VisitAggregator<VendorTree::tree_t::node_t, StatsRates, StatsRates> visitor{global, local};
            vendor.second->accept(visitor);
            cout << "\n\nStats for " << vendor.first << endl;
            cout << local.to_string();
        } else {
            vendor.second->accept(global);
        }
    }
    cout << "\n\nGlobal stats: " << endl;
    cout << global.to_string();
}

}
