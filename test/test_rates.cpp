#include <cstring>
#include <gtest/gtest.h>
#include "src/rate.h"

using namespace code_directory;

TEST(Rate, created_empty) {
    Rate new_element;
    EXPECT_TRUE(new_element.is_empty());
}

TEST(Rate, constructed) {
    Rate elem(rate_string{"1.2345"}, 500, 1000);

    EXPECT_EQ(std::string(elem.rate), "1.2345");
    EXPECT_EQ(elem.effective_date, 500);
    EXPECT_EQ(elem.end_date, 1000);
    EXPECT_FALSE(elem.is_empty());
}

TEST(rate_string, values) {
    rate_string a{"0.15"};
    EXPECT_EQ(std::string(a), "0.15");
    a.set("1.");
    EXPECT_EQ(std::string(a), "1.");
    a.set("0.0010000");
    EXPECT_EQ(std::string(a), "0.001");
    a.set("5.123456789");
    EXPECT_EQ(std::string(a), "5.123456789");
    EXPECT_THROW(a.set("5.0123456789"), std::invalid_argument);

    EXPECT_THROW(a.set("1"), std::invalid_argument);
    EXPECT_THROW(a.set("-1"), std::invalid_argument);
    EXPECT_THROW(a.set("1.0A"), std::invalid_argument);
    EXPECT_THROW(a.set(""), std::invalid_argument);
    EXPECT_THROW(a.set("1+"), std::invalid_argument);
}
