#pragma once

#include <string>
#include "types.h"

namespace code_directory {

/// Maximum length of code string is 16 symbols plus '\0'
constexpr size_t MAX_CODE_LENGTH = 16;

inline std::string trim(const std::string &str) {
    auto first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
        return "";
    }
    auto last = str.find_last_not_of(' ');
    return str.substr(first, (last-first+1));
}

/// Code type.
class code_string {
public:
    code_string() = default;
    code_string(const char *str) {
        set(str);
    }

    void set(const std::string &raw) {
        std::string str = trim(raw);
        if (str.length() > MAX_CODE_LENGTH) {
            throw std::invalid_argument("Invalid code value");
        }
        if (str.find_first_not_of("0123456789") != std::string::npos) {
            throw std::invalid_argument("Invalid code value");
        }
        value = str;
    }

    size_t length() const {
        return value.length();
    }

    code_string substr(size_t pos = 0, size_t count = std::string::npos) const {
        code_string ret;
        ret.value = value.substr(pos, count);
        return ret;
    }

    char operator[](size_t index) const {
        return value.at(index) - '0';
    }

    operator std::string() const {
        return value;
    }

    bool operator==(const code_string &other) const {
        return value == other.value;
    }

    bool operator<(const code_string &other) const {
        return value < other.value;
    }
private:
    std::string value;
};

template<>
inline code_string get_empty() {
    return code_string{};
}


typedef std::string codename_t;
}
