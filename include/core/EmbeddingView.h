#pragma once

#include <cstdint>

namespace novadb
{

struct EmbeddingView
{
    const float* data;
    uint32_t size;

    EmbeddingView()
        : data(nullptr), size(0)
    {
    }

    EmbeddingView(const float* ptr, uint32_t s)
        : data(ptr), size(s)
    {
    }

    const float& operator[](uint32_t index) const
    {
        return data[index];
    }
};

}