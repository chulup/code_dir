#pragma once

#include <cstring>
#include <iomanip>
#include <sstream>
#include "types.h"

namespace code_directory {

/**
     Maximum length of rate string as set up by DB is 30.10, 41 symbol. I will store it as uint64_t, keeping 18 digits at most plus dot.
    */
constexpr size_t MAX_RATE_LENGTH = 19;
constexpr ptrdiff_t MAX_PART_LENGTH = 9;

struct rate_string {
    static constexpr uint64_t empty = std::numeric_limits<uint64_t>::max();
    uint64_t value;
    rate_string() :
        value(empty)
    {
    }

    rate_string(const char *str) {
        set(str);
    }

    void set(const char *str_raw) {
        auto len = std::strlen(str_raw);
        // minimum accepted string contains digit and dot
        if (len < 2 || len > MAX_RATE_LENGTH) {
            throw std::invalid_argument("Invalid rate value");
        }
        if (!std::isdigit(str_raw[0])) {
            throw std::invalid_argument("Invalid rate value");
        }
        const char *last_digit = str_raw + len-1;
        while (last_digit > str_raw && *last_digit == '0') {
            --last_digit;
        }
        char str[MAX_RATE_LENGTH+1];
        len = last_digit-str_raw+1;
        std::memcpy(str, str_raw, len);
        str[len] = '\0';

        auto dot=std::strchr(str, '.');
        size_t low_len = len - (dot - str + 1); // dot-str=high_len
        if (dot == nullptr || 
            (dot - str) > MAX_PART_LENGTH ||   // Length before dot
            low_len > MAX_PART_LENGTH) // Length after dot
        {
            throw std::invalid_argument("Invalid rate value");
        }
        char *end = nullptr;

        uint64_t high = 0, low = 0;
        high = std::strtoul(str, &end, 10);
        if (end != dot) {
            throw std::invalid_argument("Invalid rate value");
        }
        if (low_len > 0) {
            // We have some number after dot. To normalize it we need
            // to make sure "1.01" and "1.010000" will
            // make an identical value.
            // Solution: multiply converted number by some power of ten,
            // where "some" is count of necessary trailing zeros to make
            // 9 digits after dot.
            low = std::strtoul(dot+1, &end, 10);
            if (end != str + len) {
                throw std::invalid_argument("Invalid rate value");
            }
            static constexpr uint64_t multipliers[] = {
                0,          // whatever, we have no digits after dot
                100000000,  // 10^8
                10000000,   // 10^7
                1000000,    // 10^6
                100000,     // 10^5
                10000,      // 10^4
                1000,       // 10^3
                100,        // 10^2
                10,         // 10^1
                1,          // 10^0
            };
            low = low * multipliers[low_len];
        }

        value = (high << 32) | low;
    }

    void set_empty(){
        value = empty;
    }

    bool is_empty() const {
        return value == empty;
    }

    operator std::string() const {
        if (is_empty()) {
            return "";
        }
        std::stringstream out;
        uint32_t high = value >> 32;
        uint32_t low = value & 0xFFFFFFFF;
        out << high << '.';
        size_t i = 1;
        static constexpr uint64_t multipliers[] = {
            0,          // whatever, we have no digits after dot
            100000000,  // 10^8
            10000000,   // 10^7
            1000000,    // 10^6
            100000,     // 10^5
            10000,      // 10^4
            1000,       // 10^3
            100,        // 10^2
            10,         // 10^1
            1,          // 10^0
        };
        while (low < multipliers[i]) {
            ++i;
            out << "0";
        }
        out << low;
        auto str = out.str();
        auto end = str.end(), begin = str.begin();
        while (end != begin && *(end-1)=='0') {
            --end;
        }
        str.erase(end, str.end());
        return str;
    }
};

bool inline operator!=(const rate_string &left, const rate_string &right) {
    return left.value != right.value;
}
bool inline operator==(const rate_string &left, const rate_string &right) {
    return left.value == right.value;
}
bool inline operator<(const rate_string &left, const rate_string &right) {
    return left.value < right.value;
}

struct Rate{
    rate_string rate;
    time_t effective_date;
    time_t end_date;
    Rate() {
        set_empty();
    }

    Rate(const rate_string &_rate, time_t _effective_date, time_t _end_date) :
        rate(_rate),
        effective_date(_effective_date),
        end_date(_end_date)
    {
    }

    Rate(const Rate& original) {
        set(original.rate, original.effective_date, original.end_date);
    }

    void set(const rate_string &_rate, time_t _effective_date, time_t _end_date) {
        rate = _rate;
        effective_date = _effective_date;
        end_date = _end_date;
    }

    void set_empty() {
        rate.set_empty();
    }
    bool is_empty() const {
        return rate.is_empty();
    }
};
}
