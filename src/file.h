#ifndef FILE_H
#define FILE_H

#include <cstdio>
#include <fstream>
#include <optional>
#include <sstream>
#include <string>

struct File {
    static std::optional<std::string> readFile(const std::string& path)
    {
        std::ifstream file(path);
        if (!file.is_open()) {
            return std::nullopt;
        }

        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    static bool exists(const std::string& path)
    {
        std::fstream file(path);
        return file.good();
    }
};

#endif // FILE_H
