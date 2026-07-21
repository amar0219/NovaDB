#include <iostream>
#include <vector>

#include "core/VectorDB.h"

using namespace novadb;

int main()
{
    VectorDB db;

    //--------------------------------------------------
    // Insert sample vectors
    //--------------------------------------------------

    db.insert(1, {1.0f, 0.0f, 0.0f});

    db.insert(2, {0.0f, 1.0f, 0.0f});

    db.insert(3, {0.0f, 0.0f, 1.0f});

    db.insert(4, {0.9f, 0.1f, 0.0f});

    db.insert(5, {0.8f, 0.2f, 0.0f});

    //--------------------------------------------------
    // Query Vector
    //--------------------------------------------------

    std::vector<float> queryVector =
    {
        1.0f,
        0.0f,
        0.0f
    };

    EmbeddingView query(
        queryVector.data(),
        static_cast<uint32_t>(queryVector.size())
    );

    //--------------------------------------------------
    // Search
    //--------------------------------------------------

    std::vector<SearchResult> results =
        db.search(query, 3);

    //--------------------------------------------------
    // Print Results
    //--------------------------------------------------

    std::cout << "Top 3 Results\n";
    std::cout << "--------------------------\n";

    for(const auto& result : results)
    {
        std::cout
            << "ID    : " << result.id << '\n'
            << "Score : " << result.score << "\n\n";
    }

    return 0;
}