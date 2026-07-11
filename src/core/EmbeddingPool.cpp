#include"core/EmbeddingPool.h"

#include<algorithm>
#include<stdexcept>

namespace novadb
{

uint64_t EmbeddingPool::insert(const std::vector<float>& embedding)
{
    const uint64_t offset=embeddings_.size();

    embeddings_.insert(
        embeddings_.end(),
        embedding.begin(),
        embedding.end()
    );

    return offset;
}

EmbeddingView EmbeddingPool::get(uint64_t offset, uint64_t dimension) const
{
    if(offset+dimension>embeddings_.size())
    {
        throw std::out_of_range("Invalid embedding offset.");
    }

    return EmbeddingView(
        embeddings_.data()+offset,
        dimension
    );
}

size_t EmbeddingPool::size() const
{
    return embeddings_.size();
}

void EmbeddingPool::clear() 
{
    embeddings_.clear();
}

const std::vector<float>& EmbeddingPool::getData() const
{
    return embeddings_;
}

void EmbeddingPool::setData(const std::vector<float>& data)
{
    embeddings_ = data;
}

}
