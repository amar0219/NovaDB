#include "search/SearchEngine.h"
#include "search/Similarity.h"
#include "search/TopKHeap.h"
namespace novadb
{
    std::vector<SearchResult> SearchEngine::search(const EmbeddingPool& pool, const std::unordered_map<uint64_t, Record>& records, const EmbeddingView& query, uint32_t k) const
    {
        TopKHeap heap(k);

        for(const auto& entry:records)
        {
            const auto& record = entry.second;

            if(!record.active)
            {
                continue;
            }

            EmbeddingView embedding = pool.get(record.embeddingOffset, record.dimension);

            float score = Similarity::cosineSimilarity(query, embedding);

            heap.push(SearchResult(record.id, score));
        }
        return heap.getResults();
    }
}