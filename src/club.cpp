#include "club.hpp"

#include <algorithm>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#include "utils.hpp"

static std::string formatTime(int32_t t) {
    char buf[16];
    snprintf(buf, sizeof(buf), "%02d:%02d", t / 60, t % 60);
    return buf;
}

void ClubEngine::freeTable(int32_t t, int32_t now) {
    auto& table = tables[t];
    int32_t duration = now - table.start_time;
    table.total_minutes += duration;
    table.revenue += ((duration + 59) / 60) * price;
    table.busy = false;
}

void ClubEngine::processEvent(const Event& e) {
    log.push_back(formatTime(e.time) + " " + std::to_string(e.id) + " " +
                  e.body[0] + (e.body.size() > 1 ? " " + e.body[1] : ""));

    if (e.id == 1) {
        if (e.time < start_time || e.time >= end_time) {
            log.push_back(formatTime(e.time) + " 13 NotOpenYet");
            return;
        }
        if (clients.count(e.body[0])) {
            log.push_back(formatTime(e.time) + " 13 YouShallNotPass");
            return;
        }
        clients[e.body[0]] = {};
    } else if (e.id == 2) {
        if (!clients.count(e.body[0])) {
            log.push_back(formatTime(e.time) + " 13 ClientUnknown");
            return;
        }
        int32_t t = std::stoi(e.body[1]) - 1;
        if (t < 0 || t >= tables_count || tables[t].busy) {
            log.push_back(formatTime(e.time) + " 13 PlaceIsBusy");
            return;
        }
        auto& c = clients[e.body[0]];
        if (c.table != -1) freeTable(c.table, e.time);

        tables[t].busy = true;
        tables[t].start_time = e.time;
        c.table = t;
        c.start_time = e.time;
    } else if (e.id == 3) {
        for (auto& t : tables)
            if (!t.busy) {
                log.push_back(formatTime(e.time) + " 13 ICanWaitNoLonger!");
                return;
            }
        if (wait_queue.size() >= tables_count) {
            log.push_back(formatTime(e.time) + " 11 " + e.body[0]);
            clients.erase(e.body[0]);
        } else {
            wait_queue.push(e.body[0]);
        }
    } else if (e.id == 4) {
        if (!clients.count(e.body[0])) {
            log.push_back(formatTime(e.time) + " 13 ClientUnknown");
            return;
        }
        auto c = clients[e.body[0]];
        if (c.table != -1) {
            freeTable(c.table, e.time);
            if (!wait_queue.empty()) {
                auto next = wait_queue.front();
                wait_queue.pop();
                clients[next].table = c.table;
                clients[next].start_time = e.time;
                tables[c.table].busy = true;
                tables[c.table].start_time = e.time;
                log.push_back(formatTime(e.time) + " 12 " + next + " " +
                              std::to_string(c.table + 1));
            }
        }
        clients.erase(e.body[0]);
    }
}

void ClubEngine::finishDay() {
    std::vector<std::string> names;
    for (auto& [n, _] : clients) names.push_back(n);
    std::sort(names.begin(), names.end());

    for (auto& n : names) {
        auto& c = clients[n];
        if (c.table != -1) freeTable(c.table, end_time);
        log.push_back(formatTime(end_time) + " 11 " + n);
    }
}

const std::vector<TableStat>& ClubEngine::getTables() const { return tables; }

const std::vector<std::string>& ClubEngine::getLog() const { return log; }

void ClubStats::process(const std::vector<std::string>& data) {
    auto times = StringUtils::split(data[1]);
    int32_t start, end;
    TimeParser::parse(times[0], &start);
    TimeParser::parse(times[1], &end);

    ClubEngine engine(std::stoul(data[0]), start, end, std::stoul(data[2]));

    std::cout << formatTime(start) << "\n";

    int32_t t;
    for (size_t i = 3; i < data.size(); ++i) {
        auto p = StringUtils::split(data[i]);
        TimeParser::parse(p[0], &t);
        Event e{.time = t,
                .id = std::stoul(p[1]),
                .body = {p.begin() + 2, p.end()}};
        engine.processEvent(e);
    }

    engine.finishDay();

    for (auto& s : engine.getLog()) std::cout << s << "\n";

    std::cout << formatTime(end) << "\n";

    size_t idx = 1;
    for (auto& t : engine.getTables()) {
        std::cout << idx++ << " " << t.revenue << " "
                  << formatTime(t.total_minutes) << "\n";
    }
}
