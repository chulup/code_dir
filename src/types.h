#pragma once

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <vector>

namespace code_directory {

template<class T>
inline bool is_empty(const T& object) {
    return object.is_empty();
}

template<class T>
inline T get_empty();

template<class T>
inline void set_empty(T& object);


template<>
inline bool is_empty(const std::string &string) {
    return string.empty();
}
template<>
inline std::string get_empty() {
    return "";
}
template<>
inline void set_empty(std::string &time) {
    time.clear();
}

/// Vendor Type
typedef int VendorId;

inline VendorId str_to_vendor(const std::string &str){
    VendorId id = 0;
    size_t len;
    if (std::sscanf(str.c_str(), "%d%zn", &id, &len) == EOF ||
        len != str.length()) {
        throw std::invalid_argument("Invalid vendor ID");
    }
    return id;
}

typedef std::int64_t time_t;
template<>
inline bool is_empty(const time_t &time) {
    return time == std::numeric_limits<time_t>::max();
}
template<>
inline time_t get_empty() {
    return std::numeric_limits<time_t>::max();
}
template<>
inline void set_empty(time_t &time) {
    time = std::numeric_limits<time_t>::max();
}

}

#include "rate.h"
#include "codename.h"
