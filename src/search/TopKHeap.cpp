#include "search/TopKHeap.h"

#include <algorithm>

namespace novadb
{
    TopKHeap::TopKHeap(std::size_t k) : k_(k)
    {
    }
     
    void TopKHeap::push(const SearchResult& result)
    {
        if(heap_.size()<k_)
        {
            heap_.push(result);
            return;
        }

        if(result.score > heap_.top().score)
        {
            heap_.pop();
            heap_.push(result);
        }
    }

    std::vector<SearchResult> TopKHeap::getResults()
    {
        std::vector<SearchResult> results;

        while(!heap_.empty())
        {
            results.push_back(heap_.top());
            heap_.pop();
        }

        std::reverse(results.begin(), results.end());

        return results;
    }
}
