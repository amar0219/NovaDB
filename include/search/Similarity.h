#pragma once

#include "core/EmbeddingView.h"

namespace novadb
{
    class Similarity
    {
    public:
        static float dotProduct(const EmbeddingView& a, const EmbeddingView& b);
        
        static float cosineSimilarity(const EmbeddingView& a, const EmbeddingView& b);

        static float euclideanDistance(const EmbeddingView& a, const EmbeddingView& b);

    };
}