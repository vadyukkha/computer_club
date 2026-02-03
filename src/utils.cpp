#include "utils.hpp"

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

std::vector<std::string> StringUtils::split(const std::string& s) {
    std::stringstream ss(s);
    std::vector<std::string> out;
    std::string word;
    while (ss >> word) out.push_back(word);
    return out;
}

bool StringUtils::isPositiveUInt(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s)
        if (!std::isdigit(c)) return false;
    try {
        return std::stoul(s) > 0;
    } catch (...) {
        return false;
    }
}

bool StringUtils::isValidClientName(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (std::isupper(c)) return false;
        if (!(std::isalnum(c) || c == '_' || c == '-')) return false;
    }
    return true;
}

bool TimeParser::parse(const std::string& s, int32_t* minutes) {
    if (s.size() != 5 || s[2] != ':') return false;
    if (!std::isdigit(s[0]) || !std::isdigit(s[1]) || !std::isdigit(s[3]) ||
        !std::isdigit(s[4]))
        return false;

    int32_t h = (s[0] - '0') * 10 + (s[1] - '0');
    int32_t m = (s[3] - '0') * 10 + (s[4] - '0');

    if (h > 23 || m > 59) return false;

    *minutes = h * 60 + m;
    return true;
}
