#pragma once

#include <cstddef>
#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

namespace novadb
{

    class FileManager
    {
    public:
        explicit FileManager(const std::string& filePath);

        bool exists() const;

        void create();

        void clear();

        void write(const std::vector<std::uint8_t>& buffer);

        std::vector<std::uint8_t> read() const;

        std::uint64_t fileSize() const;
    
    private:
        std::string filePath_;
    };
}
