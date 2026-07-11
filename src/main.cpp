#include <iostream>
#include <vector>

#include "core/VectorDB.h"

int main()
{
    novadb::VectorDB db;

    db.insert(1, {1.0f, 2.0f, 3.0f});
    db.insert(2, {4.0f, 5.0f, 6.0f});
    db.insert(3, {7.0f, 8.0f, 9.0f});

    std::cout << "Inserted 3 vectors\n\n";

    novadb::EmbeddingView embedding = db.getEmbedding(2);

    std::cout << "Embedding of Record 2: ";

    for(uint32_t i = 0; i < embedding.size; i++)
    {
        std::cout << embedding[i] << " ";
    }

    std::cout << "\n\n";

    db.update(2, {10.0f, 11.0f, 12.0f});

    embedding = db.getEmbedding(2);

    std::cout << "After Update: ";

    for(uint32_t i = 0; i < embedding.size; i++)
    {
        std::cout << embedding[i] << " ";
    }

    std::cout << "\n\n";

    db.erase(3);

    novadb::Record record;

    if(!db.get(3, record))
    {
        std::cout << "Record 3 deleted.\n";
    }
    else
    {
        std::cout << "Record still exists.\n";
    }

    return 0;
}