#pragma once

#include <queue>
#include <vector>

#include "search/SearchResult.h"

namespace novadb
{
    class TopKHeap
    {
    public:
        explicit TopKHeap(std::size_t k);

        void push(const SearchResult& result);

        std::vector<SearchResult> getResults();

    private:
        
        struct Compare
        {
            bool operator()(const SearchResult& a, const SearchResult& b) const
            {
                return a.score > b.score;

            }
        };

        std::priority_queue<
            SearchResult,
            std::vector<SearchResult>,
            Compare
        > heap_;

        std::size_t k_;
    };
}