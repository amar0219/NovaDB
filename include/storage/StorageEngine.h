#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "core/Record.h"

namespace novadb
{

class StorageEngine
{
public:
    StorageEngine(const std::string& recordsFile,
                  const std::string& embeddingsFile);

    void save(const std::unordered_map<uint64_t, Record>& records,
                const std::vector<float>& embeddings);

    void load(std::unordered_map<uint64_t, Record>& records,
                std::vector<float>& embeddings);

private:
    std::string recordsFile_;
    std::string embeddingsFile_;
};

}