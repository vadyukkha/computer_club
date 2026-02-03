#include <gtest/gtest.h>

#include "utils.hpp"

TEST(StringUtilsTest, PositiveUIntValid) {
    EXPECT_TRUE(StringUtils::isPositiveUInt("1"));
    EXPECT_TRUE(StringUtils::isPositiveUInt("123"));
}

TEST(StringUtilsTest, PositiveUIntInvalid) {
    EXPECT_FALSE(StringUtils::isPositiveUInt("0"));
    EXPECT_FALSE(StringUtils::isPositiveUInt("-1"));
    EXPECT_FALSE(StringUtils::isPositiveUInt("12a"));
    EXPECT_FALSE(StringUtils::isPositiveUInt("x"));
    EXPECT_FALSE(StringUtils::isPositiveUInt(""));
}

TEST(StringUtilsTest, ClientNameValid) {
    EXPECT_TRUE(StringUtils::isValidClientName("client1"));
    EXPECT_TRUE(StringUtils::isValidClientName("client-1"));
    EXPECT_TRUE(StringUtils::isValidClientName("client_1"));
    EXPECT_TRUE(StringUtils::isValidClientName("abc-123"));
}

TEST(StringUtilsTest, ClientNameInvalid) {
    EXPECT_FALSE(StringUtils::isValidClientName("Client1"));
    EXPECT_FALSE(StringUtils::isValidClientName("CLIENT1"));
    EXPECT_FALSE(StringUtils::isValidClientName(""));
    EXPECT_FALSE(StringUtils::isValidClientName("abc!"));
}

TEST(TimeParserTest, ValidTime) {
    int32_t minutes;
    EXPECT_TRUE(TimeParser::parse("08:30", &minutes));
    EXPECT_EQ(minutes, 8 * 60 + 30);
}

TEST(TimeParserTest, InvalidTimeFormat) {
    int32_t minutes;
    EXPECT_FALSE(TimeParser::parse("", &minutes));
    EXPECT_FALSE(TimeParser::parse("abc", &minutes));
    EXPECT_FALSE(TimeParser::parse("8:30", &minutes));
    EXPECT_FALSE(TimeParser::parse("25:00", &minutes));
    EXPECT_FALSE(TimeParser::parse("10:60", &minutes));
    EXPECT_FALSE(TimeParser::parse("aa:bb", &minutes));
}
