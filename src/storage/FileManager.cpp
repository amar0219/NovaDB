#include "storage/FileManager.h"

#include <fstream>
#include <stdexcept>

namespace novadb
{

FileManager::FileManager(const std::string& filePath)
    : filePath_(filePath)
{
}

bool FileManager::exists() const
{
    std::ifstream file(filePath_, std::ios::binary);
    return file.good();
}

void FileManager::create()
{
    std::ofstream file(filePath_, std::ios::binary);

    if (!file)
    {
        throw std::runtime_error("Failed to create file.");
    }
}

void FileManager::clear()
{
    std::ofstream file(filePath_, std::ios::binary | std::ios::trunc);

    if (!file)
    {
        throw std::runtime_error("Failed to clear file.");
    }
}

void FileManager::write(const std::vector<uint8_t>& buffer)
{
    std::ofstream file(filePath_, std::ios::binary | std::ios::trunc);

    if (!file)
    {
        throw std::runtime_error("Failed to open file for writing.");
    }

    file.write(
        reinterpret_cast<const char*>(buffer.data()),
        static_cast<std::streamsize>(buffer.size()));

    if (!file)
    {
        throw std::runtime_error("Failed while writing file.");
    }
}

std::vector<uint8_t> FileManager::read() const
{
    std::ifstream file(filePath_, std::ios::binary);

    if (!file)
    {
        throw std::runtime_error("Failed to open file for reading.");
    }

    // Find file size
    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size < 0)
    {
        throw std::runtime_error("Invalid file size.");
    }

    std::vector<uint8_t> buffer(static_cast<size_t>(size));

    if (size > 0)
    {
        file.read(
            reinterpret_cast<char*>(buffer.data()),
            size);

        if (!file)
        {
            throw std::runtime_error("Failed while reading file.");
        }
    }

    return buffer;
}

uint64_t FileManager::fileSize() const
{
    std::ifstream file(filePath_, std::ios::binary | std::ios::ate);

    if (!file)
    {
        throw std::runtime_error("Failed to open file.");
    }

    return static_cast<uint64_t>(file.tellg());
}

} // namespace novadb