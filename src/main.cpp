#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "club.hpp"
#include "validator.hpp"

int main(int argc, char **argv) {
    // parsing args
    if (argc < 2) {
        std::cerr << "Usage: main <path/to/file>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];

    // parsing file
    std::ifstream input_file(filepath);
    if (!input_file.is_open()) {
        std::cerr << "Error: Unable to open file " << filepath << std::endl;
        return 1;
    }

    std::string line;
    std::vector<std::string> file_data;
    while (std::getline(input_file, line)) {
        if (!line.empty() && line.back() == '\r')  // remove CR
            line.pop_back();
        file_data.push_back(line);
    }
    input_file.close();

    // validate format data
    HeaderValidator header;
    EventValidator events;

    FileValidator file_validator(header, events);

    auto result = file_validator.validate(file_data);
    if (!result.valid) {
        std::cout << result.error_line << std::endl;
        return 0;
    }

    // events process
    ClubStats club;
    club.process(file_data);
    return 0;
}
