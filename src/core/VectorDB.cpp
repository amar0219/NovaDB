#include "core/VectorDB.h"

#include <stdexcept>

namespace novadb
{

VectorDB::VectorDB(): storageEngine_("records.bin","embeddings.bin")
{
}

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

bool VectorDB::save()
{
    storageEngine_.save(records_, embeddingPool_.getData());
    return true;
}

bool VectorDB::load()
{
    std::vector<float> embeddings;

    storageEngine_.load(records_, embeddings);

    embeddingPool_.setData(embeddings);

    return true;
}


std::vector<SearchResult> VectorDB::search(
    const EmbeddingView& query,
    uint32_t k) const
{
    SearchEngine engine;

    return engine.search(
        embeddingPool_,
        records_,
        query,
        k
    );
}

const EmbeddingPool& VectorDB::getEmbeddingPool() const
{
    return embeddingPool_;
}

const std::unordered_map<uint64_t, Record>&
VectorDB::getRecords() const
{
    return records_;
}


} // namespace novadb