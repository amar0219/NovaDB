#pragma once
#include<cstdint>

namespace novadb
{
    struct Record
    {
        // unique id
        uint64_t id=0;


        // starting index inside EmbeddingPool
        uint64_t embeddingOffset=0;


        // dimension (number of floats in embedding vector)
        uint64_t dimension=0;


        // reserved for future metadata storage
        uint64_t metadataOffset=0;

        // active flag,
        uint8_t active=1;

    };
}