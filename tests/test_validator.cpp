#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "validator.hpp"

TEST(HeaderValidatorTest, ValidHeader) {
    HeaderValidator v;
    std::vector<std::string> data = {"10", "08:00 22:00", "100"};

    auto res = v.validate(data);
    EXPECT_TRUE(res.valid);
}

TEST(HeaderValidatorTest, InvalidTables) {
    HeaderValidator v;
    std::vector<std::string> data = {"0", "08:00 22:00", "100"};

    auto res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "0");
}

TEST(HeaderValidatorTest, InvalidTimeOrder) {
    HeaderValidator v;
    std::vector<std::string> data = {"5", "22:00 08:00", "100"};

    auto res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "22:00 08:00");
}

TEST(EventValidatorTest, ValidEvents) {
    EventValidator v;
    std::vector<std::string> data = {"10", "08:00 22:00", "100",
                                     "08:10 1 client1", "08:20 1 client2"};

    auto res = v.validate(data);
    EXPECT_TRUE(res.valid);
}

TEST(EventValidatorTest, EventTimeBackward) {
    EventValidator v;
    std::vector<std::string> data = {"10", "08:00 22:00", "100",
                                     "08:20 1 client1", "08:10 1 client2"};

    auto res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "08:10 1 client2");
}

TEST(EventValidatorTest, InvalidClientName) {
    EventValidator v;
    std::vector<std::string> data = {"10", "08:00 22:00", "100",
                                     "08:10 1 client!"};

    auto res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "08:10 1 client!");
}

TEST(EventValidatorTest, InvalidTableNumber) {
    EventValidator v;
    std::vector<std::string> data = {"10", "08:00 22:00", "100",
                                     "08:10 2 client 200"};

    auto res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "08:10 2 client 200");
}

TEST(EventValidatorTest, InvalidSample) {
    EventValidator v;
    std::vector<std::string> data = {"10", "08:00 22:00", "100", "asd"};

    auto res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "asd");

    data[3] = "asd asd";
    res = v.validate(data);
    EXPECT_FALSE(res.valid);
    EXPECT_EQ(res.error_line, "asd asd");
}

TEST(FileValidatorTest, FullValidFile) {
    std::vector<std::string> data = {"3",
                                     "09:00 19:00",
                                     "10",
                                     "08:48 1 client1",
                                     "09:41 1 client1",
                                     "09:48 1 client2",
                                     "09:52 3 client1",
                                     "09:54 2 client1 1",
                                     "10:25 2 client2 2",
                                     "10:58 1 client3",
                                     "10:59 2 client3 3",
                                     "11:30 1 client4",
                                     "11:35 2 client4 2",
                                     "11:45 3 client4",
                                     "12:33 4 client1",
                                     "12:43 4 client2",
                                     "15:52 4 client4"};

    HeaderValidator header;
    EventValidator events;
    FileValidator file(header, events);

    auto res = file.validate(data);
    EXPECT_TRUE(res.valid);
}
