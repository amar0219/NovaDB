#include "core/VectorDB.h"

#include <stdexcept>

namespace novadb
{

bool VectorDB::insert(uint64_t id,
                      const std::vector<float>& embedding)
{
    if (records_.find(id) != records_.end())
    {
        return false;
    }

    Record record;

    record.id = id;
    record.dimension = static_cast<uint32_t>(embedding.size());
    record.embeddingOffset = embeddingPool_.insert(embedding);
    record.active = 1;

    records_[id] = record;

    return true;
}

bool VectorDB::erase(uint64_t id)
{
    std::unordered_map<uint64_t, Record>::iterator it = records_.find(id);

    if (it == records_.end())
    {
        return false;
    }

    it->second.active = 0;

    return true;
}

bool VectorDB::update(uint64_t id,
                      const std::vector<float>& embedding)
{
    std::unordered_map<uint64_t, Record>::iterator it = records_.find(id);

    if (it == records_.end())
    {
        return false;
    }

    it->second.embeddingOffset = embeddingPool_.insert(embedding);
    it->second.dimension = static_cast<uint32_t>(embedding.size());
    it->second.active = 1;

    return true;
}

bool VectorDB::get(uint64_t id, Record& record) const
{
    std::unordered_map<uint64_t, Record>::const_iterator it =
        records_.find(id);

    if (it == records_.end())
    {
        return false;
    }

    if (!it->second.active)
    {
        return false;
    }

    record = it->second;

    return true;
}

EmbeddingView VectorDB::getEmbedding(uint64_t id) const
{
    std::unordered_map<uint64_t, Record>::const_iterator it =
        records_.find(id);

    if (it == records_.end())
    {
        throw std::runtime_error("Record not found.");
    }

    if (!it->second.active)
    {
        throw std::runtime_error("Record deleted.");
    }

    return embeddingPool_.get(
        it->second.embeddingOffset,
        it->second.dimension);
}

} // namespace novadb