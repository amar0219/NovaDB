#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>

#include "core/EmbeddingPool.h"
#include "core/EmbeddingView.h"
#include "core/Record.h"

#include "storage/StorageEngine.h"
#include "search/SearchResult.h"
#include "search/SearchEngine.h"
namespace novadb
{

class VectorDB
{
public:

    VectorDB();

    bool insert(uint64_t id,
                const std::vector<float>& embedding);

    uint64_t insert(const std::vector<float>& embedding);

    bool erase(uint64_t id);

    bool update(uint64_t id,
                const std::vector<float>& embedding);

    bool get(uint64_t id,
             Record& record) const;
    
    std::vector<SearchResult> search(
        const EmbeddingView& query,
        uint32_t k) const;

    bool save();

    bool load();

    EmbeddingView getEmbedding(uint64_t id) const;

    const EmbeddingPool& getEmbeddingPool() const;

    const std::unordered_map<uint64_t, Record>& getRecords() const;


private:

    EmbeddingPool embeddingPool_;

    std::unordered_map<uint64_t, Record> records_;

    StorageEngine storageEngine_;

    uint64_t nextId_ = 1;
};

}