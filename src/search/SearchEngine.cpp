
#include "search/SearchEngine.h"
#include "search/Similarity.h"
#include "search/TopKHeap.h"

#include <chrono>
#include <iostream>
#include <iomanip>

namespace novadb
{
    std::vector<SearchResult> SearchEngine::search(
        const EmbeddingPool& pool,
        const std::unordered_map<uint64_t, Record>& records,
        const EmbeddingView& query,
        uint32_t k
    ) const
    {
        TopKHeap heap(k);

        auto start = std::chrono::steady_clock::now();

        for (const auto& entry : records)
        {
            const auto& record = entry.second;

            if (!record.active)
                continue;

            EmbeddingView embedding =
                pool.get(record.embeddingOffset, record.dimension);

            float score =
                Similarity::cosineSimilarity(query, embedding);

            heap.push(SearchResult(record.id, score));
        }

        auto end = std::chrono::steady_clock::now();

        double elapsed_us =
            std::chrono::duration<double, std::micro>(
                end - start
            ).count();

        std::cout << std::fixed << std::setprecision(2)
                  << "\nSearch time: "
                  << elapsed_us << " microseconds\n";

        return heap.getResults();
    }
}
