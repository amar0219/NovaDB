#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>

#include "core/EmbeddingPool.h"
#include "core/EmbeddingView.h"
#include "core/Record.h"

namespace novadb
{

class VectorDB
{
public:

    bool insert(uint64_t id,
                const std::vector<float>& embedding);

    bool erase(uint64_t id);

    bool update(uint64_t id,
                const std::vector<float>& embedding);

    bool get(uint64_t id,
             Record& record) const;

    EmbeddingView getEmbedding(uint64_t id) const;

private:

    EmbeddingPool embeddingPool_;

    std::unordered_map<uint64_t, Record> records_;
};

}