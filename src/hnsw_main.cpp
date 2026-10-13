#include <cstdint>
#include <iostream>
#include <vector>

#include "core/VectorDB.h"
#include "search/HNSWIndex.h"

int main()
{
    novadb::VectorDB db;

    db.insert(1, {1.0f, 0.0f, 0.0f});
    db.insert(2, {0.0f, 1.0f, 0.0f});
    db.insert(3, {0.0f, 0.0f, 1.0f});

    novadb::HNSWIndex::Config config;
    config.M = 16;
    config.efConstruction = 100;
    config.efSearch = 50;

    novadb::HNSWIndex index(config);

    const auto getVector = [&db](uint64_t id) {
        return db.getEmbedding(id);
    };

    // Build the HNSW graph from the current database vectors.
    for (uint64_t id : {1ULL, 2ULL, 3ULL}) {
        index.add(id, db.getEmbedding(id), getVector);
    }

    std::cout << "NovaDB HNSW Search Pipeline\n";
    std::cout << "Indexed vectors: " << index.size() << "\n\n";

    const auto query = db.getEmbedding(1);
    const auto results = index.search(query, 3, getVector);

    std::cout << "Top-k search results:\n";

    for (const auto& result : results) {
        std::cout << "ID: " << result.id
                  << " | Cosine similarity: " << result.score
                  << '\n';
    }

    return 0;
}
