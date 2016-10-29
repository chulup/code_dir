#pragma once

#include <unordered_map>
#include <set>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>

#include "types.h"
#include "vendor_tree.h"
#include "codename_tree.h"

namespace code_directory {

class CodeDirectory {
public:
    struct rates_result_s {
        rates_result_s(const code_string &_code, const rate_string &_rate) :
            code(_code),
            rate(_rate)
        {}
        code_string code;
        rate_string rate;
    };
    typedef std::vector<rates_result_s> rates_result_t;

    struct vendors_result_s {
        vendors_result_s(VendorId _vendor,
                         const rate_string &_min,
                         const rate_string &_max) :
            vendor(_vendor),
            min(_min),
            max(_max)
        {}
        VendorId vendor;
        rate_string min;
        rate_string max;
    };
    typedef std::vector<vendors_result_s> vendors_result_t;

    // boost::shared_ptr provides atomic access to the data
    typedef boost::shared_ptr<const VendorTree> tree_pointer_t;
    typedef boost::shared_ptr<const CodenameTree> codename_pointer_t;

    CodeDirectory();

    void set_vendor_tree(VendorId vendor, tree_pointer_t tree);
    void set_codename_tree(codename_pointer_t tree);

    void remove_vendor(VendorId vendor);

    rates_result_t get_rates(VendorId vendor,
                             const codename_t &code_name,
                             rate_string *min_rate,
                             rate_string *max_rate) const;
    rates_result_t get_rates(const std::string &vendor,
                             const std::string &code_name,
                             rate_string *min_rate,
                             rate_string *max_rate) const;

    std::vector<VendorId> list_vendors() const;
    std::vector<std::string> list_codenames() const;

    vendors_result_t get_vendors(const codename_t &code_name) const;

    void print_stats(bool print_all = false) const;

private:
    std::unordered_map<VendorId, tree_pointer_t> _vendors;
    codename_pointer_t _codenames;
};

inline bool operator==(const CodeDirectory::rates_result_s &left,
                       const CodeDirectory::rates_result_s &right) {
    return left.code == right.code && left.rate == right.rate;
}
inline bool operator<(const CodeDirectory::rates_result_s &left,
                      const CodeDirectory::rates_result_s &right) {
    return left.code < right.code ||
          (left.code == right.code && left.rate < right.rate);
}

inline bool operator==(const CodeDirectory::vendors_result_s &left,
                       const CodeDirectory::vendors_result_s &right) {
    return left.vendor == right.vendor
        && left.min == right.min
        && left.max == right.max;
}

inline bool operator<(const CodeDirectory::vendors_result_s &left,
                      const CodeDirectory::vendors_result_s &right) {
    return left.vendor < right.vendor
        || (left.vendor == right.vendor &&
            left.min < right.min)
        || (left.vendor == right.vendor &&
            left.min == right.min && 
            left.max < right.max);
}
}
