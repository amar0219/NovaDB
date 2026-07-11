#pragma once

#include<cstdint>
#include<vector>
#include<cstddef>
#include "core/EmbeddingView.h"

namespace novadb
{

class EmbeddingPool{
public:

    // insert embedding, returns the starting offset of the embedding in the pool
    
    uint64_t insert(const std::vector<float>& embedding);

    // read embedding without copying
    
    EmbeddingView get(uint64_t offset, uint64_t dimension) const;

    // total float stored
    size_t size() const;

    // remove all embeddings
    void clear();

    const std::vector<float>& getData() const;

    void setData(const std::vector<float>& data);

private:
    std::vector<float> embeddings_;
    
};

}