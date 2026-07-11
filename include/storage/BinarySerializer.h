#pragma once

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include <vector>

#include "core/Record.h"

namespace novadb
{
    class BinarySerializer
    {
    public:
        
        static std::vector<std::uint8_t> serializeRecords(const std::unordered_map<uint64_t, Record>& records);

        
        static std::unordered_map<uint64_t, Record> deserializeRecords(const std::vector<std::uint8_t>& buffer);


        static std::vector<std::uint8_t> serializeEmbeddings(const std::vector<float>& embeddings);

        static std::vector<float> deserializeEmbeddings(const std::vector<std::uint8_t>& buffer);

        
    };
}