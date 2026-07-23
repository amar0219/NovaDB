#include "storage/BinarySerializer.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

namespace novadb
{

namespace
{
    template <typename T>
    void writePrimitive(std::vector<std::uint8_t>& buffer, const T& value)
    {
        static_assert(
            std::is_trivially_copyable<T>::value,
            "Type must be trivially copyable"
        );
        const auto* ptr = reinterpret_cast<const std::uint8_t*>(&value);

        buffer.insert(buffer.end(), ptr, ptr + sizeof(T));
    }

    template <typename T>
    T readPrimitive(const std::vector<std::uint8_t>& buffer, size_t& offset)
    {
        static_assert(
            std::is_trivially_copyable<T>::value,
            "Type must be trivially copyable"
        );

        if(offset + sizeof(T) > buffer.size())
            throw std::runtime_error("currupted buffer: not enough bytes to read");

        T value;

        std::memcpy(
            &value,
            buffer.data() + offset,
            sizeof(T)
        );

        offset += sizeof(T);

        return value;
    }
}// anonymous namespace

// ----------------------------------------------------------------------------------------- //

std::vector<std::uint8_t> BinarySerializer::serializeRecords(const std::unordered_map<uint64_t, Record>& records)
{
    std::vector<std::uint8_t> buffer;

    std::vector<Record> sortedRecords;

    sortedRecords.reserve(records.size());

    for(const auto& entry : records)
    {

        sortedRecords.push_back(entry.second);
    }

    std::sort(
        sortedRecords.begin(),
        sortedRecords.end(),
        [](const Record& a, const Record& b)
        {
            return a.id < b.id;
        }
    );

    writePrimitive<uint64_t>(
        buffer,
        static_cast<uint64_t>(sortedRecords.size())
    );

    for(const auto& record : sortedRecords)
    {
        writePrimitive(buffer, record.id);
        writePrimitive(buffer, record.embeddingOffset);
        writePrimitive(buffer, record.dimension);
        writePrimitive(buffer, record.metadataOffset);
        writePrimitive(buffer, record.active);
    }

    return buffer;
}

// ----------------------------------------------------------------------------------------- //

std::unordered_map<uint64_t, Record> BinarySerializer::deserializeRecords(const std::vector<std::uint8_t>& buffer)
{
    std::unordered_map<uint64_t, Record> records;

    std::size_t offset = 0;

    const uint64_t count = readPrimitive<uint64_t>(buffer, offset);

    for(uint64_t i = 0; i < count; ++i)
    {
        Record record;

        record.id = readPrimitive<uint64_t>(buffer, offset);

        record.embeddingOffset = readPrimitive<uint64_t>(buffer, offset);

        record.dimension = readPrimitive<uint64_t>(buffer, offset);

        record.metadataOffset = readPrimitive<uint64_t>(buffer, offset);

        record.active = readPrimitive<uint8_t>(buffer, offset);

        records.emplace(record.id, record);
    }

    return records;
}

// ----------------------------------------------------------------------------------------- //

std::vector<std::uint8_t> BinarySerializer::serializeEmbeddings(const std::vector<float>& embeddings)
{
    std::vector<std::uint8_t> buffer;

    writePrimitive<uint64_t>(
        buffer,
        static_cast<uint64_t>(embeddings.size())
    );                            

    const auto* ptr = reinterpret_cast<const std::uint8_t*>(embeddings.data());

    buffer.insert(buffer.end(), ptr, ptr + embeddings.size() * sizeof(float));
    
    return buffer;   
}

// ----------------------------------------------------------------------------------------- //

std::vector<float> BinarySerializer::deserializeEmbeddings(const std::vector<std::uint8_t>& buffer)
{
    std::size_t offset = 0;

    const uint64_t count = readPrimitive<uint64_t>(buffer, offset);


    std::vector<float> embeddings(count);
    const std::size_t bytes = count * sizeof(float);

    if(offset + bytes > buffer.size())
        throw std::runtime_error("currupted buffer: not enough bytes to read");

    std::memcpy(
        embeddings.data(),
        buffer.data() + offset,
        bytes
    );

    return embeddings;
}

}