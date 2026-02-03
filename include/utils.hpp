#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <string>
#include <vector>

class StringUtils {
   public:
    static std::vector<std::string> split(const std::string& s);
    static bool isPositiveUInt(const std::string& s);
    static bool isValidClientName(const std::string& s);
};

class TimeParser {
   public:
    static bool parse(const std::string& s, int32_t* minutes);
};

#endif  // UTILS_HPP
