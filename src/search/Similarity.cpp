#include "search/Similarity.h"

#include <cmath>
#include <stdexcept>

namespace novadb
{
    float Similarity::dotProduct(const EmbeddingView& a, const EmbeddingView& b)
    {
        if(a.size != b.size)
        {
            throw std::runtime_error("Embeddings dimensions do not match");
        }

        float result = 0.0f;

        for(uint32_t i = 0; i < a.size;i++)
        {
            result += a[i] * b[i];
        }

        return result;
    }

    float Similarity::cosineSimilarity(const EmbeddingView& a, const EmbeddingView& b)
    {
        if(a.size != b.size)
        {
            throw std::runtime_error("Embeddings dimensions do not match");
        }

        float dot = 0.0f;
        float normA = 0.0f;
        float normB = 0.0f;

        for(uint32_t i = 0; i < a.size; i++)
        {
            dot += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }

        return dot / (std::sqrt(normA) * std::sqrt(normB));
    }

    float Similarity::euclideanDistance(const EmbeddingView& a, const EmbeddingView& b)
    {
        if(a.size != b.size)
        {
            throw std::runtime_error("Embeddings dimensions do not match");
        }

        float sum = 0.0f;

        for(uint32_t i = 0; i < a.size; i++)
        {
            float diff = a[i] - b[i];
            sum += diff * diff;
        }

        return std::sqrt(sum);
    }
}
