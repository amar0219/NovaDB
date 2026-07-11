#include <iostream>

#include "core/EmbeddingView.h"
#include "search/Similarity.h"

int main()
{
    float vec1[] = {1.0f, 2.0f, 3.0f};
    float vec2[] = {4.0f, 5.0f, 6.0f};

    novadb::EmbeddingView a(vec1, 3);
    novadb::EmbeddingView b(vec2, 3);

    std::cout << "Dot Product: " << novadb::Similarity::dotProduct(a, b) << std::endl;

    std::cout << "Cosine Similarity: "
              << novadb::Similarity::cosineSimilarity(a, b)
              << std::endl;

    std::cout << "Euclidean Distance: "
              << novadb::Similarity::euclideanDistance(a, b)
              << std::endl;

    return 0;
}