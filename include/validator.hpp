#ifndef VALIDATOR_HPP
#define VALIDATOR_HPP

#include <string>
#include <vector>

#include "utils.hpp"

struct ValidationResult {
    bool valid = true;
    std::string error_line = "";

    static ValidationResult ok() { return {}; }

    static ValidationResult error(const std::string& line) {
        return {false, line};
    }
};

template <typename T>
class IValidator {
   public:
    virtual ~IValidator() = default;
    virtual ValidationResult validate(const T& data) const = 0;
};

struct HeaderData {
    int32_t start_time;
    int32_t end_time;
};

class HeaderValidator : public IValidator<std::vector<std::string>> {
   public:
    ValidationResult validate(
        const std::vector<std::string>& data) const override;
};

class EventValidator : public IValidator<std::vector<std::string>> {
   public:
    ValidationResult validate(
        const std::vector<std::string>& data) const override;
};

class FileValidator {
   public:
    FileValidator(const HeaderValidator& header, const EventValidator& events)
        : header_validator(header), event_validator(events) {}

    ValidationResult validate(const std::vector<std::string>& data) const;

   private:
    const HeaderValidator& header_validator;
    const EventValidator& event_validator;
};

#endif  // VALIDATOR_HPP
