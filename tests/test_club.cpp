#include <gtest/gtest.h>

#include <string>

#include "club.hpp"
#include "utils.hpp"

Event makeEvent(const std::string& time, size_t id,
                std::initializer_list<std::string> args) {
    int32_t t;
    TimeParser::parse(time, &t);
    return Event{t, id, args};
}

TEST(ArrivalTest, ClientArrivesSuccessfully) {
    ClubEngine engine(2, 540, 1140, 10);  // 09:00–19:00

    engine.processEvent(makeEvent("09:10", 1, {"client1"}));

    const auto& log = engine.getLog();
    ASSERT_EQ(log.size(), 1);
    EXPECT_EQ(log[0], "09:10 1 client1");
}

TEST(ArrivalTest, ClientArrivesBeforeOpen) {
    ClubEngine engine(2, 540, 1140, 10);

    engine.processEvent(makeEvent("08:50", 1, {"client1"}));

    const auto& log = engine.getLog();
    ASSERT_EQ(log.size(), 2);
    EXPECT_EQ(log[1], "08:50 13 NotOpenYet");
}

TEST(ArrivalTest, ClientArrivesTwice) {
    ClubEngine engine(2, 540, 1140, 10);

    engine.processEvent(makeEvent("09:10", 1, {"client1"}));
    engine.processEvent(makeEvent("09:20", 1, {"client1"}));

    const auto& log = engine.getLog();
    ASSERT_EQ(log.size(), 3);
    EXPECT_EQ(log[2], "09:20 13 YouShallNotPass");
}

TEST(SeatingTest, ClientSeatsSuccessfully) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 2, {"client1", "1"}));

    const auto& tables = engine.getTables();
    EXPECT_TRUE(tables[0].busy);
}

TEST(SeatingTest, ClientUnknown) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 2, {"client2", "1"}));

    const auto& log = engine.getLog();
    EXPECT_EQ(log.back(), "09:10 13 ClientUnknown");
}

TEST(SeatingTest, SeatOccupied) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:01", 1, {"client2"}));
    engine.processEvent(makeEvent("09:10", 2, {"client1", "1"}));
    engine.processEvent(makeEvent("09:15", 2, {"client2", "1"}));

    const auto& log = engine.getLog();
    EXPECT_EQ(log.back(), "09:15 13 PlaceIsBusy");
}

TEST(QueueTest, CannotWaitIfFreeTableExists) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 3, {"client1"}));

    const auto& log = engine.getLog();
    EXPECT_EQ(log.back(), "09:10 13 ICanWaitNoLonger!");
}

TEST(QueueTest, QueueOverflowGeneratesLeave) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:01", 1, {"client2"}));
    engine.processEvent(makeEvent("09:02", 2, {"client1", "1"}));
    engine.processEvent(makeEvent("09:03", 3, {"client2"}));
    engine.processEvent(makeEvent("09:04", 3, {"client3"}));

    const auto& log = engine.getLog();
    EXPECT_EQ(log.back(), "09:04 11 client3");
}

TEST(LeaveTest, ClientUnknown) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 2, {"client1", "1"}));
    engine.processEvent(makeEvent("10:00", 4, {"client2"}));

    const auto& log = engine.getLog();
    EXPECT_EQ(log.back(), "10:00 13 ClientUnknown");
}

TEST(LeaveTest, ClientLeavesAndFreesTable) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 2, {"client1", "1"}));
    engine.processEvent(makeEvent("10:00", 4, {"client1"}));

    const auto& tables = engine.getTables();
    EXPECT_FALSE(tables[0].busy);
    EXPECT_EQ(tables[0].revenue, 10);
}

TEST(LeaveTest, ClientLeavesAndFirstSeats) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:01", 1, {"client2"}));
    engine.processEvent(makeEvent("09:02", 2, {"client1", "1"}));
    engine.processEvent(makeEvent("09:03", 3, {"client2"}));
    engine.processEvent(makeEvent("09:10", 4, {"client1"}));

    const auto& log = engine.getLog();
    EXPECT_EQ(log.back(), "09:10 12 client2 1");
}

TEST(EndOfDayTest, ClientsLeaveAlphabetically) {
    ClubEngine engine(2, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client2"}));
    engine.processEvent(makeEvent("09:01", 1, {"client1"}));

    engine.finishDay();

    const auto& log = engine.getLog();
    EXPECT_EQ(log[0], "09:00 1 client2");
    EXPECT_EQ(log[1], "09:01 1 client1");
    EXPECT_EQ(log[2], "19:00 11 client1");
    EXPECT_EQ(log[3], "19:00 11 client2");
}

TEST(EndOfDayTest, RevenueCalculatedFullDay) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 2, {"client1", "1"}));

    engine.finishDay();

    const auto& table = engine.getTables()[0];
    EXPECT_EQ(table.revenue, 100);  // 10 hours × 10
}

TEST(EndOfDayTest, RevenueCalculatedOneMinute) {
    ClubEngine engine(1, 540, 1140, 10);

    engine.processEvent(makeEvent("09:00", 1, {"client1"}));
    engine.processEvent(makeEvent("09:10", 2, {"client1", "1"}));
    engine.processEvent(makeEvent("09:11", 4, {"client1"}));

    engine.finishDay();

    const auto& table = engine.getTables()[0];
    EXPECT_EQ(table.revenue, 10);
}
