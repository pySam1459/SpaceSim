#pragma once

#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <stdexcept>

/**
 * Load an entire text/binary file into an std::string.
 *
 * @param path  Path of the file to read (UTF-8 capable on Windows via <filesystem>).
 * @return      File contents as a contiguous std::string.
 * @throws      std::runtime_error if the file cannot be opened or read.
 */
inline std::string read_file_to_string(const std::filesystem::path& path)
{
    std::ifstream file(path, std::ios::binary);          // binary mode ↔ no CR/LF mangling
    if (!file)
        throw std::runtime_error("Unable to open file: " + path.string());

    std::ostringstream oss;
    oss << file.rdbuf();                                 // streambuf → string
    return oss.str();
}