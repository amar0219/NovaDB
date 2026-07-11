#pragma once

#include <cstdint>

namespace novadb
{
    struct SearchResult
    {
        uint64_t id=0;
        float score;

        SearchResult()
        {
            id = 0;
            score = 0.0f;
        }

        SearchResult(uint64_t id, float score)
        {
            this->id = id;
            this->score = score;
        }
    };
}