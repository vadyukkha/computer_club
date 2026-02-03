#ifndef CLUB_HPP
#define CLUB_HPP

#include <cstdint>
#include <map>
#include <queue>
#include <string>
#include <vector>

struct TableStat {
    bool busy = false;
    int32_t start_time = 0;
    size_t total_minutes = 0;
    size_t revenue = 0;
};

struct Event {
    int32_t time;
    size_t id;
    std::vector<std::string> body;
};

class ClubEngine {
   public:
    ClubEngine(size_t tables_count, int32_t start, int32_t end, size_t price)
        : tables_count(tables_count),
          start_time(start),
          end_time(end),
          price(price),
          tables(tables_count) {}

    void processEvent(const Event& e);
    void finishDay();

    const std::vector<TableStat>& getTables() const;
    const std::vector<std::string>& getLog() const;

   private:
    struct Client {
        int32_t table = -1;
        int32_t start_time = -1;
    };

    size_t tables_count;
    int32_t start_time;
    int32_t end_time;
    size_t price;

    std::vector<TableStat> tables;
    std::map<std::string, Client> clients;
    std::queue<std::string> wait_queue;
    std::vector<std::string> log;

    void freeTable(int32_t table, int32_t now);
};

class ClubStats {
   public:
    void process(const std::vector<std::string>& data);
};

#endif  // CLUB_HPP
