#pragma once

#include <vector>
#include <unordered_map>
#include <cstdint>

#include "core/EmbeddingPool.h"
#include "core/EmbeddingView.h"
#include "core/Record.h"

#include "SearchResult.h"

namespace novadb
{
    class SearchEngine
    {
    public:
        
        std::vector<SearchResult> search(const EmbeddingPool& pool, const std::unordered_map<uint64_t, Record>& records, const EmbeddingView& query, uint32_t k) const;
    };
}



