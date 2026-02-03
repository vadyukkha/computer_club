#include "validator.hpp"

#include <string>
#include <vector>

ValidationResult HeaderValidator::validate(
    const std::vector<std::string>& data) const {
    if (data.size() < 3) return ValidationResult::error(data[0]);

    if (!StringUtils::isPositiveUInt(data[0]))
        return ValidationResult::error(data[0]);

    auto parts = StringUtils::split(data[1]);
    if (parts.size() != 2) return ValidationResult::error(data[1]);

    int32_t start, end;
    if (!TimeParser::parse(parts[0], &start) ||
        !TimeParser::parse(parts[1], &end) || start >= end)
        return ValidationResult::error(data[1]);

    if (!StringUtils::isPositiveUInt(data[2]))
        return ValidationResult::error(data[2]);

    return ValidationResult::ok();
}

ValidationResult EventValidator::validate(
    const std::vector<std::string>& data) const {
    int32_t last_time = -1;

    for (size_t i = 3; i < data.size(); ++i) {
        auto e = StringUtils::split(data[i]);
        if (e.size() < 3) return ValidationResult::error(data[i]);

        int32_t t;
        if (!TimeParser::parse(e[0], &t) || t < last_time)
            return ValidationResult::error(data[i]);

        if (!StringUtils::isPositiveUInt(e[1]))
            return ValidationResult::error(data[i]);

        if (e[1] == "2") {
            if (e.size() != 4) return ValidationResult::error(data[i]);
            if (!StringUtils::isPositiveUInt(e[3]))
                return ValidationResult::error(data[i]);
            if (e[3] > data[0]) return ValidationResult::error(data[i]);
        }

        if (!StringUtils::isValidClientName(e[2]))
            return ValidationResult::error(data[i]);

        last_time = t;
    }

    return ValidationResult::ok();
}

ValidationResult FileValidator::validate(
    const std::vector<std::string>& data) const {
    auto res = header_validator.validate(data);
    if (!res.valid) return res;
    return event_validator.validate(data);
}
